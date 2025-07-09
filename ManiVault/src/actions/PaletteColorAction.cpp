// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PaletteColorAction.h"

#include "PaletteAction.h"

namespace mv::gui {

PaletteColorAction::PaletteColorAction(QObject* parent, const QString& title, const QPalette::ColorGroup& colorGroup /*= QPalette::ColorGroup::Normal*/, const QPalette::ColorRole& colorRole /*= QPalette::ColorRole::Window*/) :
    HorizontalGroupAction(parent, title),
    _colorGroup(QPalette::ColorGroup::Normal),
    _colorRole(QPalette::ColorRole::Window),
    _colorAction(this, "Color")
{
    setShowLabels(false);

    initialize(colorGroup, colorRole);

    auto paletteAction = dynamic_cast<PaletteAction*>(parent->parent());

    connect(paletteAction, &PaletteAction::paletteChanged, this, &PaletteColorAction::updateColorFromPalette);

    connect(&_colorAction, &ColorAction::colorChanged, this, [this, parent](const QColor& color) -> void {
        auto paletteAction = dynamic_cast<PaletteAction*>(parent->parent());

        paletteAction->getPalette().setColor(_colorGroup, _colorRole, color);
    });
}

void PaletteColorAction::initialize(const QPalette::ColorGroup& colorGroup, const QPalette::ColorRole& colorRole)
{
    _colorGroup = colorGroup;
    _colorRole  = colorRole;

    addAction(&_colorAction);
   
    _colorAction.setToolTip(QString("%1 %2 color").arg(PaletteColorRoleAction::colorGroupNames[_colorGroup], PaletteAction::colorRoleNames[_colorRole].toLower()));

    updateColorFromPalette();
}

void PaletteColorAction::updateColorFromPalette()
{
    auto paletteAction = dynamic_cast<PaletteAction*>(parent()->parent());

    _colorAction.setColor(paletteAction->getPalette().color(_colorGroup, _colorRole));
}

}
