// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ApplicationSettingsAction.h"
#include "Application.h"

#ifdef Q_OS_MACX
#include "util/MacThemeHelper.h"
#endif // Q_OS_MACX

namespace hdps
{

ApplicationSettingsAction::ApplicationSettingsAction(QObject* parent) :
    GlobalSettingsGroupAction(parent, "Application"),
    _appearanceOptionAction(nullptr)
{
    setShowLabels(true);
    
    bool themesAvailable = false;
    
#ifdef Q_OS_MACX
    themesAvailable = macDarkThemeAvailable();
#endif // Q_OS_MACX
    
    if( themesAvailable )
    {
        _appearanceOptionAction = new gui::OptionAction(this, "Appearance", QStringList({"System", "Dark", "Light"}), "System");
        _appearanceOptionAction->setDefaultWidgetFlags(gui::OptionAction::HorizontalButtons);
        _appearanceOptionAction->setSettingsPrefix(getSettingsPrefix() + "AppearanceOption");
            
        addAction(_appearanceOptionAction);
        
        const auto appearanceOptionCurrentIndexChanged = [this](const QString& currentText) -> void {
            //qDebug() << "Changing Appearance to " << currentText << ".";
            if(currentText == "System")
            {
#ifdef Q_OS_MACX
                macSetToAutoTheme();
#endif // Q_OS_MACX
            }
            else if(currentText == "Dark")
            {
#ifdef Q_OS_MACX
                macSetToDarkTheme();
#endif // Q_OS_MACX
            }
            else if(currentText == "Light")
            {
#ifdef Q_OS_MACX
                macSetToLightTheme();
#endif // Q_OS_MACX
            }
        };
        connect(_appearanceOptionAction, &gui::OptionAction::currentTextChanged, this, appearanceOptionCurrentIndexChanged);
    }
}

}
