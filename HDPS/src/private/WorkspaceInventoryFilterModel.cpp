// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkspaceInventoryFilterModel.h"

#include <QDebug>

#ifdef _DEBUG
    #define WORKSPACE_INVENTORY_FILTER_MODEL_VERBOSE
#endif

using namespace hdps::gui;

WorkspaceInventoryFilterModel::WorkspaceInventoryFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent),
    _filterTypesAction(this, "Workspace filter types", { "Built-in", "Recent" })
{
    _filterTypesAction.setConnectionPermissionsToNone();
    _filterTypesAction.setToolTip("Type of workspace");

    setRecursiveFilteringEnabled(true);

    connect(&_filterTypesAction, &OptionsAction::optionsChanged, this, &WorkspaceInventoryFilterModel::invalidate);
}

bool WorkspaceInventoryFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    /*
    if (_instantiatedPluginsOnlyAction.isChecked()) {
        if (!index.parent().isValid()) {
            if (!hasPluginInstances(index))
                return false;
        }

        if (index.parent().isValid() && !index.parent().parent().isValid()) {
            if (!hasPluginInstances(index, 1))
                return false;
        }
    }
    */

    if (filterRegularExpression().isValid()) {
        const auto key = sourceModel()->data(index.siblingAtColumn(filterKeyColumn()), filterRole()).toString();

        if (!key.contains(filterRegularExpression()))
            return false;
    }

    return true;
}

bool WorkspaceInventoryFilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    return lhs.data().toString() < rhs.data().toString();
}
