// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartPageActionsFilterModel.h"

#include <QDebug>

#ifdef _DEBUG
    #define START_PAGE_ACTIONS_FILTER_MODEL_VERBOSE
#endif

StartPageActionsFilterModel::StartPageActionsFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent)
{
    setRecursiveFilteringEnabled(true);
}

bool StartPageActionsFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
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

bool StartPageActionsFilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    return lhs.data().toString() < rhs.data().toString();
}
