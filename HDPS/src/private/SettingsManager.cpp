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
    _globalProjectsPathAction(this, "Projects"),
    _globalWorkspacesPathAction(this, "Workspaces"),
    _globalDataPathAction(this, "Data"),
    _ignoreLoadingErrorsAction(this, "Ignore loading errors")
{
    _editSettingsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("cogs"));
    _editSettingsAction.setShortcut(QKeySequence("Ctrl+G"));
    _editSettingsAction.setShortcutContext(Qt::WidgetWithChildrenShortcut);

    _globalProjectsPathAction.setConnectionPermissionsToNone();
    _globalWorkspacesPathAction.setConnectionPermissionsToNone();
    _globalDataPathAction.setConnectionPermissionsToNone();

    _globalProjectsPathAction.setSettingsPrefix("GlobalSettings/Paths/Projects", true);
    _globalWorkspacesPathAction.setSettingsPrefix("GlobalSettings/Paths/Workspaces", true);
    _globalDataPathAction.setSettingsPrefix("GlobalSettings/Paths/Data", true);
    _ignoreLoadingErrorsAction.setSettingsPrefix("GlobalSettings/IO/IgnoreLoadingErrors");

    const auto makeDirIfNotExist = [](const QString& pathToDirectory) -> void {
        QDir dir(pathToDirectory);

        if (!dir.exists())
            dir.mkpath(".");
    };

    //qDebug() << QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    //qDebug() << QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

    if (!QFileInfo(_globalProjectsPathAction.getDirectory()).exists()) {
        const auto projectsDir = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first();// +"/HDPS/Projects";

        makeDirIfNotExist(projectsDir);
        _globalProjectsPathAction.setDirectory(projectsDir);
    }
        
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
