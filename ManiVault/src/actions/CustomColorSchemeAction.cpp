// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "CustomColorSchemeAction.h"
#include "PaletteAction.h"

#ifdef _DEBUG
    #define CUSTOM_COLOR_SCHEME_ACTION_VERBOSE
#endif

namespace mv::gui {

CustomColorSchemeAction::CustomColorSchemeAction(QObject* parent, const QString& title) :
    HorizontalGroupAction(parent, title),
    _currentColorSchemeAction(this, "Custom"),
    _addColorSchemeAction(this, "Add"),
    _editColorSchemeAction(this, "Edit")
{
    setShowLabels(false);

    _currentColorSchemeAction.setStretch(1);
    _currentColorSchemeAction.setPlaceHolderString("Pick custom theme...");
    _currentColorSchemeAction.setToolTip("Pick custom theme");

    _addColorSchemeAction.setIconByName("plus");
    _addColorSchemeAction.setToolTip("Add custom theme");
    _addColorSchemeAction.setDefaultWidgetFlags(TriggerAction::WidgetFlag::Icon);

    _editColorSchemeAction.setIconByName("pen");
    _editColorSchemeAction.setToolTip("Edit custom theme");
    _editColorSchemeAction.setDefaultWidgetFlags(TriggerAction::WidgetFlag::Icon);

    connect(&_currentColorSchemeAction, &OptionAction::currentTextChanged, this, &CustomColorSchemeAction::currentCustomColorSchemeChanged);
}

void CustomColorSchemeAction::initialize()
{
    clear();

    addAction(&_currentColorSchemeAction);
    addAction(&_addColorSchemeAction);
    addAction(&_editColorSchemeAction);

    currentCustomColorSchemeChanged();
}

void CustomColorSchemeAction::currentCustomColorSchemeChanged()
{
    _editColorSchemeAction.setEnabled(mv::theme().getCustomColorSchemeNames({ AbstractThemeManager::CustomColorSchemeMode::Added }).contains(_currentColorSchemeAction.getCurrentText()));
}

}
