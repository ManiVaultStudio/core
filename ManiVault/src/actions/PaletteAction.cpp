// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PaletteAction.h"

#include "CoreInterface.h"

#include "AbstractThemeManager.h"

#include <qguiapplication.h>

using namespace mv::util;

namespace mv::gui {

QStringList PaletteAction::colorRoleNames = {
	"WindowText",
    "Button",
    "Light",
    "Midlight",
    "Dark",
    "Mid",
    "Text",
    "BrightText",
    "ButtonText",
    "Base",
    "Window",
    "Shadow",
    "Highlight",
    "HighlightedText",
    "Link",
    "LinkVisited",
    "AlternateBase",
    "NoRole",
    "ToolTipBase",
    "ToolTipText",
    "PlaceholderText",
    "Accent"
};

PaletteAction::PaletteAction(QObject* parent, const QString& title) :
    VerticalGroupAction(parent, title),
    _nameAction(this, "Name"),
    _fromThemeAction(this, "From theme"),
    _previewAction(this, "Preview...")
{
    setIconByName("palette");
    setConfigurationFlag(ConfigurationFlag::ForceCollapsedInGroup);
    setLabelSizingType(LabelSizingType::Auto);

    _fromThemeAction.setToolTip("Fill palette from current theme");
    _previewAction.setToolTip("Preview for a short period of time");

    connect(&_fromThemeAction, &TriggerAction::triggered, this, [this]() {
        setPalette(qApp->palette());
    });

    connect(&_previewAction, &TriggerAction::triggered, this, [this]() {
        _fromThemeAction.setEnabled(false);
        _previewAction.setEnabled(false);

        for (auto& _paletteColorRoleAction : _paletteColorRoleActions) {
            _paletteColorRoleAction->setEnabled(false);
        }

        mv::help().addNotification("Palette preview", "Previewing the color scheme, reverting back in 5 seconds...", StyledIcon("palette"));

        _cachedPalette = qApp->palette();

        qApp->setPalette(_palette);

        AbstractThemeManager::restyleAllWidgets();

        QTimer::singleShot(5000, this, [this]() {
            qApp->setPalette(_cachedPalette);

        	AbstractThemeManager::restyleAllWidgets();

            _fromThemeAction.setEnabled(true);
            _previewAction.setEnabled(true);

            for (auto& _paletteColorRoleAction : _paletteColorRoleActions) {
                _paletteColorRoleAction->setEnabled(true);
            }
		});
	});
}

void PaletteAction::initialize()
{
    clear();

    addAction(&_fromThemeAction);
    addAction(&_previewAction);

    int currentColorRoleIndex = 0;

    for (auto& _paletteColorRoleAction : _paletteColorRoleActions) {
        _paletteColorRoleAction = std::make_unique<PaletteColorRoleAction>(this, colorRoleNames[currentColorRoleIndex]);

        _paletteColorRoleAction->initialize(static_cast<QPalette::ColorRole>(currentColorRoleIndex));

        addAction(_paletteColorRoleAction.get());

        currentColorRoleIndex++;
    }
}

QPalette& PaletteAction::getPalette()
{
    return _palette;
}

void PaletteAction::setPalette(const QPalette& palette)
{
    _palette = palette;

    emit paletteChanged(_palette);
}

}
