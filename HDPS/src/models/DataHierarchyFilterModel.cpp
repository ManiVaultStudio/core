// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataHierarchyModel.h"
#include "DataHierarchyFilterModel.h"

#include <QDebug>

namespace mv {

DataHierarchyFilterModel::DataHierarchyFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent),
    _filterHiddenAction(this, "Filter hidden", true)
{
    setDynamicSortFilter(true);
    setRecursiveFilteringEnabled(true);

    connect(&_filterHiddenAction, &gui::ToggleAction::toggled, this, &DataHierarchyFilterModel::invalidate);
}

bool DataHierarchyFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    auto sourceStandardItemModel = static_cast<QStandardItemModel*>(sourceModel());

    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    auto modelItem = static_cast<DataHierarchyModel::Item*>(sourceStandardItemModel->itemFromIndex(index));

    if (_filterHiddenAction.isChecked() && !index.siblingAtColumn(static_cast<int>(DataHierarchyModel::Column::IsVisible)).data(Qt::EditRole).toBool())
        return false;

    if (filterRegularExpression().isValid())
        return sourceModel()->data(index, filterRole()).toString().contains(filterRegularExpression());

    return false;
}

}
