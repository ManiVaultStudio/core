// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "RecentFilesFilterModel.h"

#ifdef _DEBUG
    #define RECENT_FILES_FILTER_MODEL_VERBOSE
#endif

using namespace mv::util;

namespace mv {

RecentFilesFilterModel::RecentFilesFilterModel(QObject* parent /*= nullptr*/) :
    SortFilterProxyModel(parent)
{
    //setFilterKeyColumn(static_cast<int>(AbstractRecentFilesModel::Column::FilePath));
}

bool RecentFilesFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    if (filterRegularExpression().isValid()) {
        const auto key = sourceModel()->data(index.siblingAtColumn(filterKeyColumn()), filterRole()).toString();

        if (!key.contains(filterRegularExpression()))
            return false;
    }

    return true;
}

bool RecentFilesFilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    if (lhs.column() == 0)
        return lhs.data().toString() < rhs.data().toString();

    if (lhs.column() == 1)
        return lhs.data(Qt::EditRole).toDateTime() < rhs.data(Qt::EditRole).toDateTime();

    return false;
}

void RecentFilesFilterModel::setSourceModel(QAbstractItemModel* sourceModel)
{
    SortFilterProxyModel::setSourceModel(sourceModel);

    auto recentFilesModel = dynamic_cast<AbstractRecentFilesModel*>(sourceModel);

    connect(recentFilesModel, &AbstractRecentFilesModel::populated, this, &SortFilterProxyModel::invalidate);
}

}
