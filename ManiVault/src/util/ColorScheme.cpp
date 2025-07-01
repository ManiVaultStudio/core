// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ColorScheme.h"

#include <QMap>

namespace mv::util {

const QMap<ColorScheme::Mode, QString> ColorScheme::modeNames = {
    { Mode::BuiltIn, "Built-in" },
    { Mode::UserAdded, "User-added" }
};

ColorScheme::ColorScheme(const Mode& mode /*= Mode::BuiltIn*/, const QString& name /*= ""*/, const QString& description /*= ""*/, const QPalette& palette /*= QPalette()*/) :
    _mode(mode),
    _name(name),
    _description(description),
    _palette(palette)
{
}

ColorScheme::Mode ColorScheme::getMode() const
{
    return _mode;
}

void ColorScheme::setMode(const Mode& mode)
{
    _mode = mode;
}

QString ColorScheme::getName() const
{
    return _name;
}

void ColorScheme::setName(const QString& name)
{
    _name = name;
}

QString ColorScheme::getDescription() const
{
    return _description;
}

void ColorScheme::setDescription(const QString& description)
{
    _description = description;
}

QPalette ColorScheme::getPalette() const
{
    return _palette;
}

void ColorScheme::setPalette(const QPalette& palette)
{
    _palette = palette;
}

}
