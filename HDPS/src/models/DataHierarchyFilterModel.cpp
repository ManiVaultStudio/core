// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataHierarchyFilterModel.h"
#include "DataHierarchyModelItem.h"

#include <QDebug>

using namespace hdps;

DataHierarchyFilterModel::DataHierarchyFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent),
    _filterHidden(false)
{
    setRecursiveFilteringEnabled(true);
}

bool DataHierarchyFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    auto modelItem = static_cast<DataHierarchyModelItem*>(index.internalPointer());

    if (_filterHidden && !modelItem->isVisible())
        return false;

    if (filterRegularExpression().isValid()) {
        const auto key = sourceModel()->data(index, filterRole()).toString();
        return key.contains(filterRegularExpression());
    }
       
    return false;
}

bool DataHierarchyFilterModel::getFilterHidden() const
{
	return _filterHidden;
}

void DataHierarchyFilterModel::setFilterHidden(bool filterHidden)
{
    if (filterHidden == _filterHidden)
        return;

    _filterHidden = filterHidden;

    invalidate();
}
