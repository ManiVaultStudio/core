// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DatasetsFilterModel.h"
#include "AbstractDatasetsModel.h"

#include <QDebug>

#ifdef _DEBUG
    //#define DATASETS_FILTER_MODEL_VERBOSE
#endif

using namespace mv::gui;

namespace mv
{

DatasetsFilterModel::DatasetsFilterModel(QObject* parent /*= nullptr*/) :
    SortFilterProxyModel(parent),
    _filterFunction(),
    _useFilterFunctionAction(this, "Use filter function", false)
{
    invalidate();
}

void DatasetsFilterModel::setFilterFunction(const FilterFunction& filterFunction)
{
    _filterFunction = filterFunction;

    invalidate();
}

bool DatasetsFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    auto abstractDatasetsModel = static_cast<AbstractDatasetsModel*>(sourceModel());

    if (_useFilterFunctionAction.isChecked() && _filterFunction)
        return _filterFunction(static_cast<AbstractDatasetsModel::Item*>(abstractDatasetsModel->itemFromIndex(index))->getDataset());

    return true;
}

}
