// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DatasetsToRemoveFilterModel.h"
#include "DatasetsToRemoveModel.h"

#include <actions/ToggleAction.h>

#include <QDebug>
#include <QStandardItemModel>

using namespace mv::gui;

DatasetsToRemoveFilterModel::DatasetsToRemoveFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent)
{
    setRecursiveFilteringEnabled(true);
    setDynamicSortFilter(true);
}

bool DatasetsToRemoveFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    auto sourceStandardItemModel = static_cast<QStandardItemModel*>(sourceModel());

    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    if (filterRegularExpression().isValid())
        if (!sourceModel()->data(index, filterRole()).toString().contains(filterRegularExpression()))
            return false;

    if (!getDatasetsToRemoveModel()->getAdvancedAction().isChecked() && parent.isValid())
        return false;

    const auto visible = index.siblingAtColumn(static_cast<int>(DatasetsToRemoveModel::Column::Visible)).data(Qt::EditRole).toBool();

    if (!visible)
        return false;

    return true;
}

DatasetsToRemoveModel* DatasetsToRemoveFilterModel::getDatasetsToRemoveModel() const
{
    return static_cast<DatasetsToRemoveModel*>(sourceModel());
}
