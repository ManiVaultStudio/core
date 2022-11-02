#include "ActionHierarchyFilterModel.h"
#include "ActionHierarchyModelItem.h"

#include <QDebug>

using namespace hdps;

ActionHierarchyFilterModel::ActionHierarchyFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent),
    _filterVisibilityAction(this, "Visibility", { "Visible", "Hidden" }),
    _filterMayPublishAction(this, "May publish", { "Yes", "No" }),
    _filterMayConnectAction(this, "May connect", { "Yes", "No" }),
    _filterMayDisconnectAction(this, "May disconnect", { "Yes", "No" }),
    _removeFiltersAction(this, "Remove filters")
{
    setRecursiveFilteringEnabled(true);

    _filterVisibilityAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);
    _filterMayPublishAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);
    _filterMayConnectAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);
    _filterMayDisconnectAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);

    _filterVisibilityAction.setToolTip("Filter actions based on their visibility");
    _filterMayPublishAction.setToolTip("Filter actions based on whether they may publish");
    _filterMayConnectAction.setToolTip("Filter actions based on whether they may connect to a public action");
    _filterMayDisconnectAction.setToolTip("Filter actions based on whether they may disconnect from a public action");
    _removeFiltersAction.setToolTip("Remove all filters");

    _filterVisibilityAction.setConnectionPermissions(WidgetAction::None);
    _filterMayPublishAction.setConnectionPermissions(WidgetAction::None);
    _filterMayConnectAction.setConnectionPermissions(WidgetAction::None);
    _filterMayDisconnectAction.setConnectionPermissions(WidgetAction::None);

    connect(&_filterVisibilityAction, &OptionsAction::selectedOptionsChanged, this, &ActionHierarchyFilterModel::invalidate);
    connect(&_filterMayPublishAction, &OptionsAction::selectedOptionsChanged, this, &ActionHierarchyFilterModel::invalidate);
    connect(&_filterMayConnectAction, &OptionsAction::selectedOptionsChanged, this, &ActionHierarchyFilterModel::invalidate);
    connect(&_filterMayDisconnectAction, &OptionsAction::selectedOptionsChanged, this, &ActionHierarchyFilterModel::invalidate);
    
    connect(&_removeFiltersAction, &TriggerAction::triggered, this, [this]() -> void {
        _filterVisibilityAction.reset();
        _filterMayPublishAction.reset();
        _filterMayConnectAction.reset();
        _filterMayDisconnectAction.reset();
    });
}

bool ActionHierarchyFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    auto action = static_cast<ActionHierarchyModelItem*>(index.internalPointer())->getAction();

    if (filterRegularExpression().isValid()) {
        const auto key = sourceModel()->data(index, filterRole()).toString();
        
        if (!key.contains(filterRegularExpression()))
            return false;
    }

    std::int32_t numberOfActiveFilters = 0;
    std::int32_t numberOfMatches = 0;

    if (_filterVisibilityAction.hasSelectedOptions()) {
        numberOfActiveFilters++;

        const auto selectedOptions = _filterVisibilityAction.getSelectedOptions();

        if (selectedOptions.contains("Visible") && action->isVisible() || selectedOptions.contains("Hidden") && !action->isVisible())
            numberOfMatches++;
    }

    if (_filterMayPublishAction.hasSelectedOptions()) {
        numberOfActiveFilters++;

        const auto selectedOptions = _filterMayPublishAction.getSelectedOptions();

        if (selectedOptions.contains("Yes") && action->mayPublish(WidgetAction::Gui) || selectedOptions.contains("No") && !action->mayPublish(WidgetAction::Gui))
            numberOfMatches++;
    }

    if (_filterMayConnectAction.hasSelectedOptions()) {
        numberOfActiveFilters++;

        const auto selectedOptions = _filterMayConnectAction.getSelectedOptions();

        if (selectedOptions.contains("Yes") && action->mayConnect(WidgetAction::Gui) || selectedOptions.contains("No") && !action->mayConnect(WidgetAction::Gui))
            numberOfMatches++;
    }

    if (_filterMayDisconnectAction.hasSelectedOptions()) {
        numberOfActiveFilters++;

        const auto selectedOptions = _filterMayDisconnectAction.getSelectedOptions();

        if (selectedOptions.contains("Yes") && action->mayDisconnect(WidgetAction::Gui) || selectedOptions.contains("No") && !action->mayDisconnect(WidgetAction::Gui))
            numberOfMatches++;
    }

    return numberOfMatches == numberOfActiveFilters;
}