// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ColorSchemeAction.h"

#include "models/ColorSchemesListModel.h"

#ifdef _DEBUG
    #define COLOR_SCHEME_ACTION_VERBOSE
#endif

using namespace mv::util;

namespace mv::gui {

ColorSchemeAction::ColorSchemeAction(QObject* parent, const QString& title) :
    HorizontalGroupAction(parent, title),
    _currentColorSchemeAction(this, "Custom"),
    _addColorSchemeAction(this, "Add color scheme"),
    _removeColorSchemeAction(this, "Remove"),
    _editColorSchemeAction(this, "Edit color scheme")
{
    setShowLabels(false);

    _currentColorSchemeAction.setStretch(1);
    _currentColorSchemeAction.setPlaceHolderString("Pick custom theme...");
    _currentColorSchemeAction.setToolTip("Pick custom theme");
    
    _addColorSchemeAction.setIconByName("plus");
    _addColorSchemeAction.setToolTip("Add custom theme");
    _addColorSchemeAction.setDefaultWidgetFlags(TriggerAction::WidgetFlag::Icon);
    _addColorSchemeAction.initialize(true);

    _removeColorSchemeAction.setIconByName("trash");
    _removeColorSchemeAction.setToolTip("Remove custom theme");
    _removeColorSchemeAction.setDefaultWidgetFlags(TriggerAction::WidgetFlag::Icon);

    _editColorSchemeAction.setIconByName("pen");
    _editColorSchemeAction.setToolTip("Edit custom theme");
    _editColorSchemeAction.setDefaultWidgetFlags(TriggerAction::WidgetFlag::Icon);

    connect(&_currentColorSchemeAction, &OptionAction::currentTextChanged, this, &ColorSchemeAction::currentColorSchemeChanged);
}

void ColorSchemeAction::initialize()
{
    clear();

    addAction(&_currentColorSchemeAction);
    //addAction(&_addColorSchemeAction, GroupAction::WidgetFlag::Vertical);
    //addAction(&_removeColorSchemeAction);
    //addAction(&_editColorSchemeAction);

    _currentColorSchemeAction.setCustomModel(const_cast<ColorSchemesListModel*>(&static_cast<const AbstractThemeManager&>(mv::theme()).getCustomColorSchemeListModel()));

    currentColorSchemeChanged();
}

void ColorSchemeAction::currentColorSchemeChanged()
{
    _editColorSchemeAction.setEnabled(false);//mv::theme().getCustomColorSchemeNames({ ColorScheme::Mode::UserAdded }).contains(_currentColorSchemeAction.getCurrentText()));
    _removeColorSchemeAction.setEnabled(false);//mv::theme().getCustomColorSchemeNames({ ColorScheme::Mode::UserAdded }).contains(_currentColorSchemeAction.getCurrentText()));
}

}
