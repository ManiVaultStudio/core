// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SettingsManager.h"
#include "SettingsManagerDialog.h"

#include <Application.h>

#include <util/Exception.h>

#include <QStandardPaths>

using namespace hdps::gui;
using namespace hdps::util;

#ifdef _DEBUG
    #define SETTINGS_MANAGER_VERBOSE
#endif

namespace hdps
{

SettingsManager::SettingsManager() :
    AbstractSettingsManager(),
    _editSettingsAction(this, "Settings..."),
    _parametersSettingsAction(this),
    _miscellaneousSettingsAction(this)
{
    _editSettingsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("cogs"));
    _editSettingsAction.setShortcut(QKeySequence("Ctrl+G"));
    _editSettingsAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);
        
    connect(&_editSettingsAction, &TriggerAction::triggered, this, &SettingsManager::edit);

    auto mainWindow = Application::topLevelWidgets().first();

    mainWindow->addAction(&_editSettingsAction);
}

void SettingsManager::initialize()
{
#ifdef SETTINGS_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    AbstractManager::initialize();
}

void SettingsManager::reset()
{
#ifdef SETTINGS_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    beginReset();
    {
    }
    endReset();
}

void SettingsManager::edit()
{
    auto* dialog = new SettingsManagerDialog();
    connect(dialog, &SettingsManagerDialog::finished, dialog, &SettingsManagerDialog::deleteLater);
    dialog->open();
}

}
