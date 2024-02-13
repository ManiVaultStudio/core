// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DatasetsFilterModel.h"
#include "DatasetsModel.h"

#include <QDebug>

DatasetsFilterModel::DatasetsFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    setRecursiveFilteringEnabled(true);
}

bool DatasetsFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    auto sourceStandardItemModel = static_cast<QStandardItemModel*>(sourceModel());

    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    if (filterRegularExpression().isValid()) {
        const auto key = index.siblingAtColumn(static_cast<DatasetsModel::Column>(filterKeyColumn())).data(filterRole()).toString();

        if (!key.contains(filterRegularExpression()))
            return false;
    }

    return true;
}
