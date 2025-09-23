// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AbstractColorSchemesModel.h"

using namespace mv;
using namespace mv::plugin;

#ifdef _DEBUG
    #define ABSTRACT_COLOR_SCHEMES_MODEL_VERBOSE
#endif

using namespace mv::util;

namespace mv {

AbstractColorSchemesModel::Item::Item(util::ColorScheme& colorScheme) :
    _colorScheme(colorScheme)
{
    setEditable(false);
}

ColorScheme& AbstractColorSchemesModel::Item::getColorScheme()
{
    return _colorScheme;
}

const util::ColorScheme& AbstractColorSchemesModel::Item::getColorScheme() const
{
    return _colorScheme;
}

QVariant AbstractColorSchemesModel::NameItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getColorScheme().getName();

        case Qt::ToolTipRole:
            return QString("Name: %1").arg(data(Qt::DisplayRole).toString());

        case Qt::DecorationRole:
            return getColorScheme().getMode() == ColorScheme::Mode::UserAdded ? StyledIcon("pen") : QIcon();

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractColorSchemesModel::DescriptionItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getColorScheme().getName();

        case Qt::ToolTipRole:
            return QString("Description: %1").arg(data(Qt::DisplayRole).toString());

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractColorSchemesModel::PaletteItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getColorScheme().getPalette();

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractColorSchemesModel::ModeItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return static_cast<int>(getColorScheme().getMode());

        case Qt::DisplayRole:
                return data(Qt::EditRole).toInt() == static_cast<int>(ColorScheme::Mode::BuiltIn) ? "Built-int" : "User-added";

        case Qt::ToolTipRole:
            return QString("Mode: %1").arg(data(Qt::DisplayRole).toString());

        default:
            break;
    }

    return Item::data(role);
}

AbstractColorSchemesModel::AbstractColorSchemesModel(QObject* parent /*= nullptr*/) :
    StandardItemModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));
}

QVariant AbstractColorSchemesModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    switch (static_cast<Column>(section))
    {
        case Column::Name:
            return NameItem::headerData(orientation, role);

        case Column::Description:
            return DescriptionItem::headerData(orientation, role);

        case Column::Palette:
            return PaletteItem::headerData(orientation, role);

        case Column::Mode:
            return ModeItem::headerData(orientation, role);

        default:
            break;
    }

    return {};
}

}