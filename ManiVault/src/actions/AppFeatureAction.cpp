// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AppFeatureAction.h"

namespace mv::gui {

AppFeatureAction::AppFeatureAction(QObject* parent, const QString& title) :
    HorizontalGroupAction(parent, title),
    _enabledAction(this, "Enabled", false),
    _settingsAction(this, "Settings")
{
    setShowLabels(false);

    _settingsAction.setIconByName("gear");
    _settingsAction.setToolTip(QString("%1 settings").arg(title));
    _settingsAction.setConfigurationFlag(ConfigurationFlag::ForceCollapsedInGroup);

    HorizontalGroupAction::addAction(&_enabledAction);
    HorizontalGroupAction::addAction(&_settingsAction);

    const auto updateSettingsActionReadOnly = [this]() -> void {
        _settingsAction.setEnabled(_enabledAction.isChecked());
	};

    updateSettingsActionReadOnly();

    connect(&_enabledAction, &ToggleAction::toggled, this, updateSettingsActionReadOnly);
}

void AppFeatureAction::addAction(WidgetAction* action, std::int32_t widgetFlags /*= -1*/, WidgetConfigurationFunction widgetConfigurationFunction /*= WidgetConfigurationFunction()*/, bool load)
{
    _settingsAction.addAction(action, widgetFlags, widgetConfigurationFunction, load);
}
}
