// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AppFeatureAction.h"

namespace mv::gui {

AppFeatureAction::AppFeatureAction(QObject* parent, const QString& title) :
    HorizontalGroupAction(parent, title),
    _settingsAction(this, "Settings")
{
    _settingsAction.setIconByName("gear");
    _settingsAction.setToolTip(QString("%1 settings").arg(title));

    HorizontalGroupAction::addAction(&_settingsAction);
}

void AppFeatureAction::addAction(WidgetAction* action, std::int32_t widgetFlags, WidgetConfigurationFunction widgetConfigurationFunction, bool load)
{
    _settingsAction.addAction(action, widgetFlags, widgetConfigurationFunction, load);
}
}
