// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SettingsManagerDialog.h"

#include <CoreInterface.h>
#include <QOperatingSystemVersion>
#include <QVBoxLayout>

using namespace mv;
using namespace mv::util;

#ifdef _DEBUG
    #define SETTINGS_MANAGER_DIALOG_VERBOSE
#endif

namespace mv::gui {

SettingsManagerDialog::SettingsManagerDialog(QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _groupsAction(this, "Groups")
{
    setWindowIcon(StyledIcon("gears"));
    setWindowModality(Qt::ApplicationModal);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setWindowTitle("Settings");
    setMinimumWidth(800);

    auto layout = new QVBoxLayout();

    setLayout(layout);

    layout->addWidget(_groupsAction.createWidget(this));

#ifdef ERROR_LOGGING
    _groupsAction.addGroupAction(&mv::settings().getErrorLoggingSettingsAction());
#endif

    _groupsAction.addGroupAction(&mv::settings().getAppearanceSettingsAction());
    _groupsAction.addGroupAction(&mv::settings().getParametersSettings());
    _groupsAction.addGroupAction(&mv::settings().getMiscellaneousSettings());
    _groupsAction.addGroupAction(&mv::settings().getTasksSettingsAction());
    _groupsAction.addGroupAction(&mv::settings().getTemporaryDirectoriesSettingsAction());

    for (auto pluginFactory : mv::plugins().getPluginFactoriesByTypes()) {
        if (auto pluginGlobalSettingsGroupAction = pluginFactory->getGlobalSettingsGroupAction())
            _groupsAction.addGroupAction(pluginGlobalSettingsGroupAction);
    }
}

QSize SettingsManagerDialog::sizeHint() const
{
    return { 400, 500 };
}

}
