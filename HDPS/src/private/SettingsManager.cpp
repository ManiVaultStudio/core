#include "SettingsManager.h"
#include "SettingsManagerDialog.h"

#include <Application.h>

#include <util/Exception.h>

using namespace hdps::gui;
using namespace hdps::util;

#ifdef _DEBUG
    #define SETTINGS_MANAGER_VERBOSE
#endif

namespace hdps
{

SettingsManager::SettingsManager() :
    AbstractSettingsManager(),
    _editSettingsAction(this, "Edit"),
    _globalProjectsPathAction(this, "Projects"),
    _globalWorkspacesPathAction(this, "Workspaces"),
    _globalDataPathAction(this, "Data")
{
    _editSettingsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("cogs"));
    _editSettingsAction.setShortcut(QKeySequence("Ctrl+P"));
    _editSettingsAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);

    connect(&_editSettingsAction, &TriggerAction::triggered, this, &SettingsManager::edit);

    auto mainWindow = Application::topLevelWidgets().first();

    mainWindow->addAction(&_editSettingsAction);
}

void SettingsManager::initalize()
{
#ifdef SETTINGS_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif
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
    SettingsManagerDialog settingsManagerDialog;

    settingsManagerDialog.exec();
}

}
