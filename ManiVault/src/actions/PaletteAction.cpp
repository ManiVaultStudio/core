// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PaletteAction.h"

namespace mv::gui {

QStringList PaletteAction::colorRoleNames = {
    "AlternateBase",
    "Base",
    "Button",
    "ButtonText",
    "Dark",
    "Highlight",
    "HighlightedText",
    "Light",
    "Link",
    "LinkVisited",
    "Mid",
    "Midlight",
    "Shadow",
    "Text",
    "Window",
    "WindowText"
};

PaletteAction::PaletteAction(QObject* parent, const QString& title) :
    VerticalGroupAction(parent, title),
    _nameAction(this, "Name")
{
    setIconByName("palette");
    setConfigurationFlag(ConfigurationFlag::ForceCollapsedInGroup);
}

void PaletteAction::initialize()
{
    clear();

    int currentColorRoleIndex = 0;

    for (auto& _paletteColorRoleAction : _paletteColorRoleActions) {
        _paletteColorRoleAction = std::make_unique<PaletteColorRoleAction>(this, colorRoleNames[currentColorRoleIndex]);

        addAction(_paletteColorRoleAction.get());

        currentColorRoleIndex++;
    }
}

}
