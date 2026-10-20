// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "EditColorSchemeAction.h"

#ifdef _DEBUG
    #define EDIT_COLOR_SCHEME_ACTION_VERBOSE
#endif

using namespace mv::util;

namespace mv::gui {

EditColorSchemeAction::EditColorSchemeAction(QObject* parent, const QString& title) :
    VerticalGroupAction(parent, title),
    _nameAction(this, "Name"),
    _paletteAction(this, "Edit color palette"),
    _updateAction(this, "Update")
{
    setConfigurationFlag(ConfigurationFlag::ForceCollapsedInGroup);
    setIconByName("pen");
    setDefaultWidgetFlags(Vertical);

    //_currentColorSchemeAction.setStretch(1);
    //_currentColorSchemeAction.setPlaceHolderString("Pick custom theme...");
    //_currentColorSchemeAction.setToolTip("Pick custom theme");
    //
    //_addColorSchemeAction.setIconByName("plus");
    //_addColorSchemeAction.setToolTip("Add custom theme");
    //_addColorSchemeAction.setDefaultWidgetFlags(TriggerAction::WidgetFlag::Icon);

    //_removeColorSchemeAction.setIconByName("trash");
    //_removeColorSchemeAction.setToolTip("Remove custom theme");
    //_removeColorSchemeAction.setDefaultWidgetFlags(TriggerAction::WidgetFlag::Icon);

    //_editColorSchemeAction.setIconByName("pen");
    //_editColorSchemeAction.setToolTip("Edit custom theme");
    //_editColorSchemeAction.setDefaultWidgetFlags(TriggerAction::WidgetFlag::Icon);

    //connect(&_currentColorSchemeAction, &OptionAction::currentTextChanged, this, &CustomColorSchemeAction::currentColorSchemeChanged);
}

void EditColorSchemeAction::initialize(bool add /*= false*/)
{
    clear();

    addAction(&_nameAction);
    addAction(&_paletteAction);
    addAction(&_updateAction);

    _paletteAction.initialize();
    //_currentColorSchemeAction.setCustomModel(const_cast<ColorSchemesListModel*>(&static_cast<const AbstractThemeManager&>(mv::theme()).getCustomColorSchemeListModel()));

    //currentColorSchemeChanged();
}

}
