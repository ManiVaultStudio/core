#include "FileMenu.h"
#include "PluginManagerDialog.h"

#include <Application.h>

using namespace hdps;
using namespace hdps::gui;

FileMenu::FileMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent),
    _newProjectAction(this, "New Project"),
    _openProjectAction(this, "Open Project"),
    _saveProjectAction(this, "Save Project"),
    _saveProjectAsAction(this, "Save Project As..."),
    _recentProjectsMenu(this),
    _workspaceMenu(this),
    _resetMenu(this),
    _importDataMenu(this),
    _publishAction(this, "Publish"),
    _pluginManagerAction(this, "Plugin Manager"),
    _globalSettingsAction(this, "Global &Settings..."),
    _startPageAction(this, "Start page"),
    _exitAction(this, "Exit")
{
    setTitle("File");
    setToolTip("File operations");

    addAction(&_newProjectAction);
    addAction(&_openProjectAction);
    addAction(&_saveProjectAction);
    addAction(&_saveProjectAsAction);

    addSeparator();

    addMenu(&_workspaceMenu);
    
    addSeparator();
    
    addMenu(&_recentProjectsMenu);
    
    addSeparator();

#ifdef _DEBUG
    addSeparator();
    {
        addMenu(&_resetMenu);
    }
    addSeparator();
#endif
    
    addMenu(&_importDataMenu);
    addSeparator();
    addAction(&_publishAction);
    addSeparator();
    addAction(&_pluginManagerAction); 
    addSeparator();
    addAction(&_globalSettingsAction);
    addSeparator();
    addAction(&_startPageAction);
    addAction(&_exitAction);

    _newProjectAction.setShortcut(QKeySequence("Ctrl+N"));
    _newProjectAction.setIcon(Application::getIconFont("FontAwesome").getIcon("file"));
    _newProjectAction.setToolTip("Open project from disk");

    _openProjectAction.setShortcut(QKeySequence("Ctrl+O"));
    _openProjectAction.setIcon(Application::getIconFont("FontAwesome").getIcon("folder-open"));
    _openProjectAction.setToolTip("Open project from disk");

    _saveProjectAction.setShortcut(QKeySequence("Ctrl+S"));
    _saveProjectAction.setIcon(Application::getIconFont("FontAwesome").getIcon("save"));
    _saveProjectAction.setToolTip("Save project to disk");

    _saveProjectAsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("save"));
    _saveProjectAsAction.setToolTip("Save project to disk in a chosen location");

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

    connect(&_openProjectAction, &QAction::triggered, this, []() -> void {
        Application::current()->loadProject();
    });

    connect(&_saveProjectAction, &QAction::triggered, [this](bool) {
        Application::current()->saveProject(Application::current()->getCurrentProjectFilePath());
    });

    connect(&_saveProjectAsAction, &QAction::triggered, [this](bool) {
        Application::current()->saveProject();
    });

    connect(&_pluginManagerAction, &TriggerAction::triggered, this, [this]() -> void {
        PluginManagerDialog loadedPluginsDialog;
        loadedPluginsDialog.exec();
    });

    connect(&_exitAction, &TriggerAction::triggered, Application::current(), &Application::quit);

    connect(this, &QMenu::aboutToShow, this, [this]() -> void {
        const auto hasDatasets = Application::core()->requestAllDataSets().size();

        _saveProjectAction.setEnabled(!Application::current()->getCurrentProjectFilePath().isEmpty());
        _saveProjectAsAction.setEnabled(hasDatasets);
        _recentProjectsMenu.updateActions();
        _saveProjectAsAction.setEnabled(true);
    });
}
