// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ColorMapFilterModel.h"
#include "ColorMapModel.h"

namespace mv::util {

ColorMapFilterModel::ColorMapFilterModel(QObject *parent, const ColorMap::Type& type /*= ColorMap::Type::OneDimensional*/) :
    QSortFilterProxyModel(parent),
    _type(type)
{
}

bool ColorMapFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto index = sourceModel()->index(row, static_cast<int>(ColorMapModel::Column::NoDimensions));
    return static_cast<int>(_type) == sourceModel()->data(index, Qt::EditRole).toInt();
}

mv::util::ColorMap::Type ColorMapFilterModel::getType() const
{
    return _type;
}

void ColorMapFilterModel::setType(const ColorMap::Type& type)
{
    if (type == _type)
        return;

    _type = type;
    invalidateFilter();
}

}
