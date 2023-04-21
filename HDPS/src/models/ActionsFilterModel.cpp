#include "ActionsFilterModel.h"
#include "ActionsModel.h"
#include "CoreInterface.h"

#include <QDebug>

using namespace hdps::gui;

namespace hdps
{

ActionsFilterModel::ActionsFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent),
    _typeFilterAction(this, "Type"),
    _typeCompleter(this),
    _scopeFilterAction(this, "Scope", { "Private", "Public" }, { "Private", "Public" }),
    _connectedFilterAction(this, "Connected", true, true),
    _filterInternalUseAction(this, "Internal", { "Yes", "No" }, { "No" }),
    _filterEnabledAction(this, "Enabled", { "Yes", "No" }),
    _filterVisibilityAction(this, "Visibility", { "Visible", "Hidden" }),
    _filterMayPublishAction(this, "May publish", { "Yes", "No" }),
    _filterMayConnectAction(this, "May connect", { "Yes", "No" }),
    _filterMayDisconnectAction(this, "May disconnect", { "Yes", "No" }),
    _removeFiltersAction(this, "Remove filters")
{
    setRecursiveFilteringEnabled(true);

    _typeFilterAction.setClearable(true);
    _typeFilterAction.setCompleter(&_typeCompleter);

    _scopeFilterAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);

    connect(&_typeFilterAction, &StringAction::stringChanged, this, &ActionsFilterModel::invalidate);
    connect(&_scopeFilterAction, &OptionsAction::selectedOptionsChanged, this, &ActionsFilterModel::invalidate);
    connect(&_connectedFilterAction, &ToggleAction::toggled, this, &ActionsFilterModel::invalidate);

    connect(&actions().getModel(), &ActionsModel::actionTypesChanged, this, [this](const QStringList& actionTypes) -> void {
        _typeFilterAction.getCompleter()->setModel(new QStringListModel(actionTypes));
    });

    _filterInternalUseAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);
    _filterEnabledAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);
    _filterVisibilityAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);
    _filterMayPublishAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);
    _filterMayConnectAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);
    _filterMayDisconnectAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);

    _filterInternalUseAction.setToolTip("Hide actions that are for internal use only");
    _filterEnabledAction.setToolTip("Filter actions based on their visibility");
    _filterVisibilityAction.setToolTip("Filter actions based on their visibility");
    _filterMayPublishAction.setToolTip("Filter actions based on whether they may publish");
    _filterMayConnectAction.setToolTip("Filter actions based on whether they may connect to a public action");
    _filterMayDisconnectAction.setToolTip("Filter actions based on whether they may disconnect from a public action");
    _removeFiltersAction.setToolTip("Remove all filters");

    const auto selectedOptionsChanged = [this]() -> void {
        invalidate();

        QList<bool> resettable;

        resettable << _filterInternalUseAction.isResettable();
        resettable << _filterEnabledAction.isResettable();
        resettable << _filterVisibilityAction.isResettable();
        resettable << _filterMayPublishAction.isResettable();
        resettable << _filterMayConnectAction.isResettable();
        resettable << _filterMayDisconnectAction.isResettable();

        _removeFiltersAction.setEnabled(resettable.contains(true));
    };

    connect(&_filterInternalUseAction, &OptionsAction::selectedOptionsChanged, this, selectedOptionsChanged);
    connect(&_filterEnabledAction, &OptionsAction::selectedOptionsChanged, this, selectedOptionsChanged);
    connect(&_filterVisibilityAction, &OptionsAction::selectedOptionsChanged, this, selectedOptionsChanged);
    connect(&_filterMayPublishAction, &OptionsAction::selectedOptionsChanged, this, selectedOptionsChanged);
    connect(&_filterMayConnectAction, &OptionsAction::selectedOptionsChanged, this, selectedOptionsChanged);
    connect(&_filterMayDisconnectAction, &OptionsAction::selectedOptionsChanged, this, selectedOptionsChanged);

    connect(&_removeFiltersAction, &TriggerAction::triggered, this, [this]() -> void {
        _filterInternalUseAction.reset();
        _filterEnabledAction.reset();
        _filterVisibilityAction.reset();
        _filterMayPublishAction.reset();
        _filterMayConnectAction.reset();
        _filterMayDisconnectAction.reset();
    });

    selectedOptionsChanged();
}

bool ActionsFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    //auto action = dynamic_cast<ActionsModel::Item*>(index.internalPointer())->getAction();

    if (filterRegularExpression().isValid()) {
        const auto key = sourceModel()->data(index.siblingAtColumn(filterKeyColumn()), filterRole()).toString();

        if (!key.contains(filterRegularExpression()))
            return false;
    }
    
    const auto typeFilter   = _typeFilterAction.getString();

    if (!typeFilter.isEmpty()) {
        const auto type = sourceModel()->data(index.siblingAtColumn(static_cast<int>(ActionsModel::Column::Type)), Qt::EditRole).toString();

        if (type != typeFilter)
            return false;
    }
    /**/

    /*
    const auto isConnected = sourceModel()->data(index.siblingAtColumn(static_cast<int>(ActionsModel::Column::Connected)), Qt::EditRole).toBool();

    if (_connectedFilterAction.isChecked() && !isConnected)
        return false;
    */

    //if (!parent.isValid()) {
        const auto scope = sourceModel()->data(index.siblingAtColumn(static_cast<int>(ActionsModel::Column::Scope)), Qt::EditRole).toInt();

        if (scope == 0 && !_scopeFilterAction.getSelectedOptionIndices().contains(0))
            return false;

        if (scope == 1 && !_scopeFilterAction.getSelectedOptionIndices().contains(1))
            return false;
    //}

        std::int32_t numberOfActiveFilters = 0;
        std::int32_t numberOfMatches = 0;

        //if (_filterInternalUseAction.hasSelectedOptions()) {
        //    numberOfActiveFilters++;

        //    const auto selectedOptions = _filterInternalUseAction.getSelectedOptions();
        //    const auto internalUseOnly = action->isConfigurationFlagSet(WidgetAction::ConfigurationFlag::InternalUseOnly);

        //    if (selectedOptions.contains("Yes") && internalUseOnly || selectedOptions.contains("No") && !internalUseOnly)
        //        numberOfMatches++;
        //}

        //if (_filterEnabledAction.hasSelectedOptions()) {
        //    numberOfActiveFilters++;

        //    const auto selectedOptions = _filterEnabledAction.getSelectedOptions();
        //    const auto isEnabled = action->isEnabled();

        //    if (selectedOptions.contains("Yes") && isEnabled || selectedOptions.contains("No") && !isEnabled)
        //        numberOfMatches++;
        //}

        //if (_filterVisibilityAction.hasSelectedOptions()) {
        //    numberOfActiveFilters++;

        //    const auto selectedOptions = _filterVisibilityAction.getSelectedOptions();
        //    const auto isVisible = action->isVisible();

        //    if (selectedOptions.contains("Visible") && isVisible || selectedOptions.contains("Hidden") && !isVisible)
        //        numberOfMatches++;
        //}

        //if (_filterMayPublishAction.hasSelectedOptions()) {
        //    numberOfActiveFilters++;

        //    const auto selectedOptions = _filterMayPublishAction.getSelectedOptions();
        //    const auto mayPublish = action->mayPublish(WidgetAction::Gui);

        //    if (selectedOptions.contains("Yes") && mayPublish || selectedOptions.contains("No") && !mayPublish)
        //        numberOfMatches++;
        //}

        //if (_filterMayConnectAction.hasSelectedOptions()) {
        //    numberOfActiveFilters++;

        //    const auto selectedOptions = _filterMayConnectAction.getSelectedOptions();
        //    const auto mayConnect = action->mayConnect(WidgetAction::Gui);

        //    if (selectedOptions.contains("Yes") && mayConnect || selectedOptions.contains("No") && !mayConnect)
        //        numberOfMatches++;
        //}

        //if (_filterMayDisconnectAction.hasSelectedOptions()) {
        //    numberOfActiveFilters++;

        //    const auto selectedOptions = _filterMayDisconnectAction.getSelectedOptions();
        //    const auto mayDisconnect = action->mayDisconnect(WidgetAction::Gui);

        //    if (selectedOptions.contains("Yes") && mayDisconnect || selectedOptions.contains("No") && !mayDisconnect)
        //        numberOfMatches++;
        //}

        return numberOfMatches == numberOfActiveFilters;
}

bool ActionsFilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    return lhs.data().toString() < rhs.data().toString();
}

WidgetAction* ActionsFilterModel::getAction(std::int32_t rowIndex)
{
    const auto sourceModelIndex = mapToSource(index(rowIndex, 0));
    return static_cast<ActionsModel*>(sourceModel())->getAction(sourceModelIndex.row());
}

}
