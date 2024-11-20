// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ActionsFilterModel.h"

#include "actions/WidgetAction.h"

#include <QDebug>

using namespace mv::gui;

namespace mv
{

ActionsFilterModel::ActionsFilterModel(QObject* parent /*= nullptr*/) :
    SortFilterProxyModel(parent),
    _typeFilterAction(this, "Type"),
    _typeFilterHumanReadableAction(this, "Type"),
    _typeCompleter(this),
    _scopeFilterAction(this, "Scope", { "Private", "Public" }, { "Private", "Public" }),
    _filterForceHiddenAction(this, "Force hidden", { "Yes", "No" }),
    _filterForceDisabledAction(this, "Force disabled", { "Yes", "No" }),
    _filterMayPublishAction(this, "May publish", { "Yes", "No" }),
    _filterMayConnectAction(this, "May connect", { "Yes", "No" }),
    _filterMayDisconnectAction(this, "May disconnect", { "Yes", "No" }),
    _publicRootOnlyAction(this, "Public root only", false),
    _removeFiltersAction(this, "Remove filters")
{
    setRecursiveFilteringEnabled(true);

    //_typeFilterHumanReadableAction.setClearable(true);
    //_typeFilterHumanReadableAction.setCompleter(&_typeCompleter);

    _scopeFilterAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);

    connect(&_typeFilterAction, &StringsAction::stringsChanged, this, &ActionsFilterModel::invalidate);
    connect(&_typeFilterHumanReadableAction, &StringsAction::stringsChanged, this, &ActionsFilterModel::invalidate);
    connect(&_scopeFilterAction, &OptionsAction::selectedOptionsChanged, this, &ActionsFilterModel::invalidate);
    connect(&_publicRootOnlyAction, &ToggleAction::toggled, this, &ActionsFilterModel::invalidate);

    //const auto updateTypeFilterActionCompleter = [this]() -> void {
    //    _typeFilterHumanReadableAction.getCompleter()->setModel(new QStringListModel(actions().getActionTypesHumanFriendly()));
    //};

    //updateTypeFilterActionCompleter();

    //connect(&actions(), &AbstractActionsManager::actionTypesHumanFriendlyChanged, this, updateTypeFilterActionCompleter);

    //_filterInternalUseAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);
    //_filterForceHiddenAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);
    //_filterForceDisabledAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);
    //_filterMayPublishAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);
    //_filterMayConnectAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);
    //_filterMayDisconnectAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);

    _filterForceHiddenAction.setToolTip("Filter parameters based whether they are force hidden");
    _filterForceDisabledAction.setToolTip("Filter parameters based whether they are force disabled");
    _filterMayPublishAction.setToolTip("Filter parameters based on whether they may publish");
    _filterMayConnectAction.setToolTip("Filter parameters based on whether they may connect to a public parameter");
    _filterMayDisconnectAction.setToolTip("Filter parameters based on whether they may disconnect from a public parameter");
    _publicRootOnlyAction.setToolTip("Filter public root or not");
    _removeFiltersAction.setToolTip("Remove all filters");

    connect(&_filterForceHiddenAction, &OptionsAction::selectedOptionsChanged, this, &ActionsFilterModel::invalidate);
    connect(&_filterForceDisabledAction, &OptionsAction::selectedOptionsChanged, this, &ActionsFilterModel::invalidate);
    connect(&_filterMayPublishAction, &OptionsAction::selectedOptionsChanged, this, &ActionsFilterModel::invalidate);
    connect(&_filterMayConnectAction, &OptionsAction::selectedOptionsChanged, this, &ActionsFilterModel::invalidate);
    connect(&_filterMayDisconnectAction, &OptionsAction::selectedOptionsChanged, this, &ActionsFilterModel::invalidate);

    invalidate();
}

bool ActionsFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    if (filterRegularExpression().isValid()) {
        const auto key = getSourceData(index, static_cast<AbstractActionsModel::Column>(filterKeyColumn()), filterRole()).toString();

        if (!key.contains(filterRegularExpression()))
            return false;
    }
    
    const auto typeFilter = _typeFilterAction.getStrings();

    if (!typeFilter.isEmpty()) {
        const auto actionType = getSourceData(index, AbstractActionsModel::Column::Type, Qt::EditRole).toString();

        if (!typeFilter.contains(actionType))
            return false;
    }

    const auto typeFilterHumanReadable = _typeFilterHumanReadableAction.getStrings();

    if (!typeFilterHumanReadable.isEmpty()) {
        const auto actionTypeHumanReadable = getSourceData(index, AbstractActionsModel::Column::Type, Qt::DisplayRole).toString();

        if (!typeFilterHumanReadable.contains(actionTypeHumanReadable))
            return false;
    }

    const auto scope = getSourceData(index, AbstractActionsModel::Column::Scope, Qt::EditRole).toInt();

    if (scope == 0 && !_scopeFilterAction.getSelectedOptionIndices().contains(0))
        return false;

    if (scope == 1 && !_scopeFilterAction.getSelectedOptionIndices().contains(1))
        return false;

    std::int32_t numberOfActiveFilters  = 0;
    std::int32_t numberOfMatches        = 0;

    if (_filterForceHiddenAction.hasSelectedOptions()) {
        numberOfActiveFilters++;

        const auto selectedOptions  = _filterForceHiddenAction.getSelectedOptions();
        const auto isForceHidden    = getSourceData(index, AbstractActionsModel::Column::ForceHidden, Qt::EditRole).toBool();

        if ((selectedOptions.contains("Yes") && isForceHidden) || (selectedOptions.contains("No") && !isForceHidden))
            numberOfMatches++;
    }

    if (_filterForceDisabledAction.hasSelectedOptions()) {
        numberOfActiveFilters++;

        const auto selectedOptions  = _filterForceDisabledAction.getSelectedOptions();
        const auto isForceDisabled  = getSourceData(index, AbstractActionsModel::Column::ForceDisabled, Qt::EditRole).toBool();

        if ((selectedOptions.contains("Yes") && isForceDisabled) || (selectedOptions.contains("No") && !isForceDisabled))
            numberOfMatches++;
    }

    if (_filterMayPublishAction.hasSelectedOptions()) {
        numberOfActiveFilters++;

        const auto selectedOptions  = _filterMayPublishAction.getSelectedOptions();
        const auto mayPublish       = getSourceData(index, AbstractActionsModel::Column::MayPublish, Qt::EditRole).toBool();

        if ((selectedOptions.contains("Yes") && mayPublish) || (selectedOptions.contains("No") && !mayPublish))
            numberOfMatches++;
    }

    if (_filterMayConnectAction.hasSelectedOptions()) {
        numberOfActiveFilters++;

        const auto selectedOptions  = _filterMayConnectAction.getSelectedOptions();
        const auto mayConnect       = getSourceData(index, AbstractActionsModel::Column::MayConnect, Qt::EditRole).toBool();

        if ((selectedOptions.contains("Yes") && mayConnect) || (selectedOptions.contains("No") && !mayConnect))
            numberOfMatches++;
    }

    if (_filterMayDisconnectAction.hasSelectedOptions()) {
        numberOfActiveFilters++;

        const auto selectedOptions  = _filterMayDisconnectAction.getSelectedOptions();
        const auto mayDisconnect    = getSourceData(index, AbstractActionsModel::Column::MayDisconnect, Qt::EditRole).toBool();

        if ((selectedOptions.contains("Yes") && mayDisconnect) || (selectedOptions.contains("No") && !mayDisconnect))
            numberOfMatches++;
    }

    if (_publicRootOnlyAction.isChecked()) {
        const auto scope = static_cast<WidgetAction::Scope>(getSourceData(index, AbstractActionsModel::Column::Scope, Qt::EditRole).toInt());

        switch (scope)
        {
            case WidgetAction::Scope::Private:
            {
                const auto parentIsPublic   = static_cast<WidgetAction::Scope>(getSourceData(parent, AbstractActionsModel::Column::Scope, Qt::EditRole).toInt()) == WidgetAction::Scope::Public;
                const auto parentIsRoot     = getSourceData(parent, AbstractActionsModel::Column::IsRoot, Qt::EditRole).toBool();

                if (!(parentIsPublic && parentIsRoot))
                    return false;

                break;
            }

            case WidgetAction::Scope::Public:
            {
                if (!getSourceData(index, AbstractActionsModel::Column::IsRoot, Qt::EditRole).toBool())
                    return false;

                break;
            }

            default:
                break;
        }
        
    }

    return numberOfMatches == numberOfActiveFilters;
}

bool ActionsFilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    return lhs.data().toString() < rhs.data().toString();
}

WidgetAction* ActionsFilterModel::getAction(std::int32_t rowIndex)
{
    const auto sourceModelIndex = mapToSource(index(rowIndex, 0));

    return static_cast<AbstractActionsModel*>(sourceModel())->getAction(sourceModelIndex.row());
}

QVariant ActionsFilterModel::getSourceData(const QModelIndex& index, const AbstractActionsModel::Column& column, int role) const
{
    return sourceModel()->data(index.siblingAtColumn(static_cast<int>(column)), role);
}

}
