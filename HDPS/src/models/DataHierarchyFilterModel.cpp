// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataHierarchyModel.h"
#include "DataHierarchyFilterModel.h"
#include "Dataset.h"
#include "Set.h"
#include "DataHierarchyItem.h"

#include <QDebug>

namespace mv {

DataHierarchyFilterModel::DataHierarchyFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent),
    _filterHidden(false)
{
    setRecursiveFilteringEnabled(true);
}

bool DataHierarchyFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    auto sourceStandardItemModel = static_cast<QStandardItemModel*>(sourceModel());

    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    auto modelItem = static_cast<DataHierarchyModel::Item*>(sourceStandardItemModel->itemFromIndex(index));

    if (_filterHidden && !modelItem->getDataset()->getDataHierarchyItem().isVisible())
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

}
