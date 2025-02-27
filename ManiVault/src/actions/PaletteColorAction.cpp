// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PaletteColorAction.h"

namespace mv::gui {

PaletteColorAction::PaletteColorAction(QObject* parent, const QString& title, const QPalette::ColorGroup& colorGroup /*= QPalette::ColorGroup::Normal*/, const QPalette::ColorRole& colorRole /*= QPalette::ColorRole::Window*/) :
    HorizontalGroupAction(parent, title),
    _colorGroup(QPalette::ColorGroup::Normal),
    _colorRole(QPalette::ColorRole::Window),
    _colorAction(this, "Color")
{
    initialize(colorGroup, colorRole);
}

void PaletteColorAction::initialize(const QPalette::ColorGroup& colorGroup, const QPalette::ColorRole& colorRole)
{
    _colorGroup = colorGroup;
    _colorRole  = colorRole;

    addAction(&_colorAction);
}

}
