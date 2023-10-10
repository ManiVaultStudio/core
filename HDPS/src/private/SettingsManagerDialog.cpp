// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SettingsManagerDialog.h"

#include <Application.h>
#include <CoreInterface.h>
#include <QVBoxLayout>

using namespace hdps;

#ifdef _DEBUG
    #define SETTINGS_MANAGER_DIALOG_VERBOSE
#endif

namespace hdps::gui {

SettingsManagerDialog::SettingsManagerDialog(QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _groupsAction(this, "Groups")
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("cogs"));
    setModal(true);
    setWindowTitle("Settings");

    auto layout = new QVBoxLayout();

    setLayout(layout);

    layout->addWidget(_groupsAction.createWidget(this));

    _groupsAction.addGroupAction(&hdps::settings().getParametersSettings());
    _groupsAction.addGroupAction(&hdps::settings().getMiscellaneousSettings());
    _groupsAction.addGroupAction(&hdps::settings().getTasksSettingsAction());
    _groupsAction.addGroupAction(&hdps::settings().getApplicationSettings());
}

QSize SettingsManagerDialog::sizeHint() const
{
    return QSize(400, 500);
}

}
