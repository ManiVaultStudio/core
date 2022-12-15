#include "FileMenu.h"
#include "PluginManagerDialog.h"
#include "HdpsApplication.h"

#include <Application.h>

using namespace hdps;
using namespace hdps::gui;

FileMenu::FileMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent),
    _importDataMenu(this),
    _publishAction(this, "Publish"),
    _pluginManagerAction(this, "Plugin Manager"),
    _globalSettingsAction(this, "Global &Settings..."),
    _startPageAction(this, "Start page"),
    _exitAction(this, "Exit")
{
    setTitle("File");
    setToolTip("File operations");

    _importDataMenu.setIcon(Application::getIconFont("FontAwesome").getIcon("file-import"));

    _publishAction.setShortcut(QKeySequence("Ctrl+P"));
    _publishAction.setIcon(Application::getIconFont("FontAwesome").getIcon("share"));
    _publishAction.setToolTip("Publish the HDPS application");

    _pluginManagerAction.setShortcut(QKeySequence("Ctrl+M"));
    _pluginManagerAction.setIcon(Application::getIconFont("FontAwesome").getIcon("plug"));
    _pluginManagerAction.setToolTip("View loaded plugins");

    _globalSettingsAction.setShortcut(QKeySequence("Ctrl+E"));
    _globalSettingsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("cogs"));
    _globalSettingsAction.setToolTip("Modify global HDPS settings");

    _startPageAction.setShortcut(QKeySequence("Alt+W"));
    _startPageAction.setIcon(Application::getIconFont("FontAwesome").getIcon("door-open"));
    _startPageAction.setToolTip("Show the HDPS start page");

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
    
//
//    addSeparator();
//
//
//    addMenu(&_importDataMenu);
//    addSeparator();
//    addAction(&_publishAction);
//    addSeparator();
//    addAction(&_pluginManagerAction);
//    addSeparator();
//    addAction(&_globalSettingsAction);
//    addSeparator();
//    addAction(&_startPageAction);
//    addAction(&_exitAction);
}
