#include "FileMenu.h"
#include "PluginManagerDialog.h"

#include <Application.h>

using namespace hdps;
using namespace hdps::gui;

FileMenu::FileMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent),
    _publishAction(this, "Publish"),
    _pluginManagerAction(this, "Plugin Manager"),
    _globalSettingsAction(this, "Global &Settings..."),
    _exitAction(this, "Exit")
{
    setTitle("File");
    setToolTip("File operations");

    _publishAction.setShortcut(QKeySequence("Ctrl+P"));
    _publishAction.setIcon(Application::getIconFont("FontAwesome").getIcon("share"));
    _publishAction.setToolTip("Publish the HDPS application");

    _pluginManagerAction.setShortcut(QKeySequence("Ctrl+M"));
    _pluginManagerAction.setIcon(Application::getIconFont("FontAwesome").getIcon("plug"));
    _pluginManagerAction.setToolTip("View loaded plugins");

    _globalSettingsAction.setShortcut(QKeySequence("Ctrl+E"));
    _globalSettingsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("cogs"));
    _globalSettingsAction.setToolTip("Modify global HDPS settings");

    _exitAction.setShortcut(QKeySequence("Alt+F4"));
    _exitAction.setIcon(Application::getIconFont("FontAwesome").getIcon("sign-out-alt"));
    _exitAction.setToolTip("Exit the HDPS application");

    connect(&_pluginManagerAction, &TriggerAction::triggered, this, [this]() -> void {
        PluginManagerDialog::managePlugins();
    });

    connect(&_exitAction, &TriggerAction::triggered, Application::current(), &Application::quit);
}

void FileMenu::showEvent(QShowEvent* showEvent)
{
    clear();

    auto& projectManager = Application::core()->getProjectManager();

    addAction(&projectManager.getNewProjectAction());
    addAction(&projectManager.getOpenProjectAction());
    addAction(&projectManager.getSaveProjectAction());
    addAction(&projectManager.getSaveProjectAsAction());
    addMenu(projectManager.getRecentProjectsMenu());
    addSeparator();
    addMenu(Application::core()->getWorkspaceManager().getMenu());
    addSeparator();
    addMenu( projectManager.getImportDataMenu());
    addSeparator();
    //addAction(&_pluginManagerAction);
    addSeparator();
    //addAction(&_globalSettingsAction);
    addSeparator();
    addAction(&projectManager.getShowStartPageAction());
    //addAction(&_exitAction);
}
