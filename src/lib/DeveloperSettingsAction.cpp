// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "DeveloperSettingsAction.h"

#include <Application.h>
#include <CoreInterface.h>

#include <util/StyledIcon.h>

namespace mv::gui
{

DeveloperSettingsAction::DeveloperSettingsAction(QObject* parent) :
    GlobalSettingsGroupAction(parent, "Developer"),
    _enableDeveloperAddonsAction(this, "Enable developer add-ons", false)
{
    setIconByName("code");

    _enableDeveloperAddonsAction.setToolTip("Enable plugin add-ons marked for developer use. Restart ManiVault Studio for this change to take effect.");
    addAction(&_enableDeveloperAddonsAction);

    const auto legacyDeveloperMenuSetting = QStringLiteral("DeveloperMenu/Visible");
    const auto newSettingPath = _enableDeveloperAddonsAction.getSettingsPrefix();

    if (!Application::current()->hasSetting(newSettingPath) && Application::current()->hasSetting(legacyDeveloperMenuSetting))
        _enableDeveloperAddonsAction.setChecked(Application::current()->getSetting(legacyDeveloperMenuSetting).toBool());

    connect(&_enableDeveloperAddonsAction, &ToggleAction::toggled, this, [this](bool enabled) {
        const auto title = QStringLiteral("Restart required");
        const auto description = enabled
            ? QStringLiteral("Developer add-ons will be loaded after restarting ManiVault Studio.")
            : QStringLiteral("Developer add-ons will be excluded after restarting ManiVault Studio. Currently loaded developer add-ons remain available until then.");

        mv::help().addNotification(title, description, util::StyledIcon("code"));
    });
}

ToggleAction& DeveloperSettingsAction::getEnableDeveloperAddonsAction()
{
    return _enableDeveloperAddonsAction;
}

bool DeveloperSettingsAction::isDeveloperAddonsEnabled() const
{
    return _enableDeveloperAddonsAction.isChecked();
}

}
