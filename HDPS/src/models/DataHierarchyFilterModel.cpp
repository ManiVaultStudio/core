// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataHierarchyModel.h"
#include "DataHierarchyFilterModel.h"

#include <QDebug>

namespace mv {

DataHierarchyFilterModel::DataHierarchyFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent),
    _visibilityFilterAction(this, "Filter visibility", { "Visible", "Hidden" }, { "Visible" })
{
    setDynamicSortFilter(true);
    setRecursiveFilteringEnabled(true);

    connect(&_visibilityFilterAction, &gui::OptionsAction::selectedOptionsChanged, this, &DataHierarchyFilterModel::invalidate);
}

bool DataHierarchyFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    auto sourceStandardItemModel = static_cast<QStandardItemModel*>(sourceModel());

    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    auto modelItem = static_cast<DataHierarchyModel::Item*>(sourceStandardItemModel->itemFromIndex(index));

    std::int32_t numberOfActiveFilters = 0;
    std::int32_t numberOfMatches = 0;

    if (_visibilityFilterAction.hasSelectedOptions()) {
        numberOfActiveFilters++;

        const auto selectedOptions  = _visibilityFilterAction.getSelectedOptions();
        const auto isVisible        = index.siblingAtColumn(static_cast<int>(DataHierarchyModel::Column::IsVisible)).data(Qt::EditRole).toBool();

        if ((selectedOptions.contains("Visible") && isVisible) || (selectedOptions.contains("Hidden") && !isVisible))
            numberOfMatches++;
    }

    return numberOfMatches == numberOfActiveFilters;
}

}
