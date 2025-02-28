// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ColorSchemesFilterModel.h"
#include "AbstractColorSchemesModel.h"

#include <QDebug>

#ifdef _DEBUG
    #define COLOR_SCHEME_FILTER_MODEL_VERBOSE
#endif

using namespace mv::util;

namespace mv {

ColorSchemesFilterModel::ColorSchemesFilterModel(QObject* parent /*= nullptr*/) :
    SortFilterProxyModel(parent),
    _modeFilterAction(this, "Mode", { "Built-in", "User-added" }, { "Built-in", "User-added" })
{
    setRecursiveFilteringEnabled(true);

    connect(&_modeFilterAction, &gui::OptionsAction::selectedOptionsChanged, this, &ColorSchemesFilterModel::invalidateFilter);
}

bool ColorSchemesFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    if (_modeFilterAction.hasSelectedOptions()) {
        const auto colorSchemeMode     = sourceModel()->data(index.siblingAtColumn(static_cast<int>(AbstractColorSchemesModel::Column::Mode)), Qt::EditRole).value<util::ColorScheme::Mode>();
        const auto colorSchemeModeName = ColorScheme::modeNames[colorSchemeMode];

        if (!_modeFilterAction.getSelectedOptions().contains(colorSchemeModeName))
            return false;
    }

	return SortFilterProxyModel::filterAcceptsRow(row, parent);
}

}
