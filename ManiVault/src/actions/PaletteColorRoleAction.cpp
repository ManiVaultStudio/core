// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PaletteColorRoleAction.h"

namespace mv::gui {

QStringList PaletteColorRoleAction::colorGroupNames = {
    "Normal",
    "Disabled",
    "Inactive"
};

PaletteColorRoleAction::PaletteColorRoleAction(QObject* parent, const QString& title, const QPalette::ColorRole& colorRole /*= QPalette::ColorRole::Window*/) :
    HorizontalGroupAction(parent, title),
    _nameAction(this, "Name")
{
    setShowLabels(false);
    initialize(colorRole);
}

void PaletteColorRoleAction::initialize(const QPalette::ColorRole& colorRole)
{
    clear();

    _colorRole = colorRole;

    int currentColorGroupIndex = 0;

    for (auto& paletteColorAction : _paletteColorActions) {
        paletteColorAction = std::make_unique<PaletteColorAction>(this, colorGroupNames[currentColorGroupIndex], static_cast<QPalette::ColorGroup>(currentColorGroupIndex), colorRole);

        addAction(paletteColorAction.get());

        currentColorGroupIndex++;
    }
}

}
