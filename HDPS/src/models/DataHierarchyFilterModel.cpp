// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataHierarchyTreeModel.h"
#include "DataHierarchyFilterModel.h"

#include <QDebug>

using namespace mv::gui;

namespace mv {

DataHierarchyFilterModel::DataHierarchyFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent),
    _visibilityFilterAction(this, "Filter visibility", { "Visible", "Hidden" }, { "Visible" }),
    _groupFilterAction(this, "Filter group", { "Yes", "No" }),
    _lockedFilterAction(this, "Filter locked", { "Yes", "No" }),
    _derivedFilterAction(this, "Filter derived", { "Yes", "No" })
{
    setDynamicSortFilter(true);
    setRecursiveFilteringEnabled(true);

    _visibilityFilterAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);
    _groupFilterAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);
    _lockedFilterAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);
    _derivedFilterAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);

    connect(&_visibilityFilterAction, &gui::OptionsAction::selectedOptionsChanged, this, &DataHierarchyFilterModel::invalidate);
    connect(&_groupFilterAction, &gui::OptionsAction::selectedOptionsChanged, this, &DataHierarchyFilterModel::invalidate);
    connect(&_lockedFilterAction, &gui::OptionsAction::selectedOptionsChanged, this, &DataHierarchyFilterModel::invalidate);
    connect(&_derivedFilterAction, &gui::OptionsAction::selectedOptionsChanged, this, &DataHierarchyFilterModel::invalidate);
}

bool DataHierarchyFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    auto sourceStandardItemModel = static_cast<QStandardItemModel*>(sourceModel());

    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    if (filterRegularExpression().isValid()) {
        const auto key = index.siblingAtColumn(static_cast<AbstractDataHierarchyModel::Column>(filterKeyColumn())).data(filterRole()).toString();

        if (!key.contains(filterRegularExpression()))
            return false;
    }

    std::int32_t numberOfActiveFilters = 0, numberOfMatches = 0;

    if (_visibilityFilterAction.hasSelectedOptions()) {
        numberOfActiveFilters++;

        const auto selectedOptions  = _visibilityFilterAction.getSelectedOptions();
        const auto isVisible        = index.siblingAtColumn(static_cast<int>(AbstractDataHierarchyModel::Column::IsVisible)).data(Qt::EditRole).toBool();

        if ((selectedOptions.contains("Visible") && isVisible) || (selectedOptions.contains("Hidden") && !isVisible))
            numberOfMatches++;
    }

    if (_groupFilterAction.hasSelectedOptions()) {
        numberOfActiveFilters++;

        const auto selectedOptions  = _visibilityFilterAction.getSelectedOptions();
        const auto isGroup          = index.siblingAtColumn(static_cast<int>(AbstractDataHierarchyModel::Column::IsGroup)).data(Qt::EditRole).toBool();

        if ((selectedOptions.contains("Yes") && isGroup) || (selectedOptions.contains("No") && !isGroup))
            numberOfMatches++;
    }

    if (_lockedFilterAction.hasSelectedOptions()) {
        numberOfActiveFilters++;

        const auto selectedOptions  = _lockedFilterAction.getSelectedOptions();
        const auto isLocked         = index.siblingAtColumn(static_cast<int>(AbstractDataHierarchyModel::Column::IsLocked)).data(Qt::EditRole).toBool();

        if ((selectedOptions.contains("Yes") && isLocked) || (selectedOptions.contains("No") && !isLocked))
            numberOfMatches++;
    }

    if (_derivedFilterAction.hasSelectedOptions()) {
        numberOfActiveFilters++;

        const auto selectedOptions  = _derivedFilterAction.getSelectedOptions();
        const auto isDerived        = index.siblingAtColumn(static_cast<int>(AbstractDataHierarchyModel::Column::IsDerived)).data(Qt::EditRole).toBool();

        if ((selectedOptions.contains("Yes") && isDerived) || (selectedOptions.contains("No") && !isDerived))
            numberOfMatches++;
    }

    return numberOfMatches == numberOfActiveFilters;
}

}
