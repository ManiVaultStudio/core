// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ClustersFilterModel.h"
#include "ClustersModel.h"

#include <QDebug>

ClustersFilterModel::ClustersFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent),
    _nameFilter("")
{
}

bool ClustersFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    if (_nameFilter.isEmpty())
        return true;

    const auto clusterName = sourceModel()->data(sourceModel()->index(row, static_cast<std::int32_t>(ClustersModel::Column::Name)), Qt::EditRole).toString();

    return clusterName.contains(_nameFilter, Qt::CaseInsensitive);
}

bool ClustersFilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    const auto lhsColumn    = static_cast<ClustersModel::Column>(lhs.column());
    const auto rhsColumn    = static_cast<ClustersModel::Column>(rhs.column());
    const auto lhsData      = sourceModel()->data(lhs, Qt::EditRole);
    const auto rhsData      = sourceModel()->data(rhs, Qt::EditRole);

    if (lhsColumn != rhsColumn)
        return false;

    switch (lhsColumn) {
        case ClustersModel::Column::Color:
            return lhsData.value<QColor>().hue() < rhsData.value<QColor>().hue();

        case ClustersModel::Column::Name:
            return lhs.row() < rhs.row();

        case ClustersModel::Column::NumberOfIndices:
            return lhsData.toInt() < rhsData.toInt();

        default:
            break;
    }

    return false;
}

QString ClustersFilterModel::getNameFilter() const
{
    return _nameFilter;
}

void ClustersFilterModel::setNameFilter(const QString& nameFilter)
{
    if (nameFilter == _nameFilter)
        return;

    _nameFilter = nameFilter;

    invalidateFilter();
}
