// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SettingsManager.h"
#include "SettingsManagerDialog.h"

#include <Application.h>
#include <CoreInterface.h>

#include <util/Exception.h>

#include <QStandardPaths>
#include <QMainWindow>
#include <QOperatingSystemVersion>

using namespace mv::gui;
using namespace mv::util;

#ifdef _DEBUG
    //#define SETTINGS_MANAGER_VERBOSE
#endif

namespace mv
{

SettingsManager::SettingsManager(QObject* parent) :
    AbstractSettingsManager(parent),
    _editSettingsAction(this, "Settings..."),
    _parametersSettingsAction(this),
    _miscellaneousSettingsAction(this),
    _tasksSettingsAction(this),
    _appearanceSettingsAction(this),
    _temporaryDirectoriesSettingsAction(this)
{
    _editSettingsAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);

    if (QOperatingSystemVersion::currentType() == QOperatingSystemVersion::MacOS) {
        _editSettingsAction.setShortcut(QKeySequence("Ctrl+,"));
        _editSettingsAction.setMenuRole(QAction::PreferencesRole);
    } else {
        //_editSettingsAction.setShortcut(QKeySequence("Ctrl+G"));
        _editSettingsAction.setIconByName("gears");
    }
        
    connect(&_editSettingsAction, &TriggerAction::triggered, this, &SettingsManager::edit);

    auto mainWindow = Application::topLevelWidgets().first();

    mainWindow->addAction(&_editSettingsAction);
}

SettingsManager::~SettingsManager()
{
    reset();
}

void SettingsManager::initialize()
{
#ifdef SETTINGS_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    AbstractSettingsManager::initialize();

    if (isInitialized())
        return;

    beginInitialization();
    endInitialization();
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
    auto* settingsManagerDialog = new SettingsManagerDialog();

    connect(settingsManagerDialog, &SettingsManagerDialog::finished, settingsManagerDialog, &SettingsManagerDialog::deleteLater);
    
    settingsManagerDialog->open();
}

PluginGlobalSettingsGroupAction* SettingsManager::getPluginGlobalSettingsGroupAction(const QString& kind)
{
    try
    {
        if (kind.isEmpty())
            throw std::runtime_error("Plugin kind is empty");

        auto pluginFactory = mv::plugins().getPluginFactory(kind);

        if (!pluginFactory)
            throw std::runtime_error("No plugin factory loaded with kind");

        return pluginFactory->getGlobalSettingsGroupAction();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get plugin global settings group action", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to get plugin global settings group action");
    }

    return {};
}

mv::gui::PluginGlobalSettingsGroupAction* SettingsManager::getPluginGlobalSettingsGroupAction(const plugin::Plugin* plugin)
{
    try
    {
        if (!plugin)
            throw std::runtime_error("Plugin is nullptr");

        const auto pluginKind = plugin->getKind();

        if (pluginKind.isEmpty())
            throw std::runtime_error("Plugin kind is empty");

        return getPluginGlobalSettingsGroupAction(pluginKind);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get plugin global settings group action", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to get plugin global settings group action");
    }

    return {};
}

}
