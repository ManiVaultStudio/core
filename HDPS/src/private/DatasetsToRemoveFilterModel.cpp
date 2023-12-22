// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DatasetsToRemoveFilterModel.h"

#include <QDebug>
#include <QStandardItemModel>

DatasetsToRemoveFilterModel::DatasetsToRemoveFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent)
{
    setRecursiveFilteringEnabled(true);
}

bool DatasetsToRemoveFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    auto sourceStandardItemModel = static_cast<QStandardItemModel*>(sourceModel());

    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    if (filterRegularExpression().isValid()) {
        const auto key = sourceModel()->data(index, filterRole()).toString();
        return key.contains(filterRegularExpression());
    }
       
    return false;
}
