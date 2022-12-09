#include "FileMenu.h"
#include "PluginManagerDialog.h"

#include <Application.h>

using namespace hdps;
using namespace hdps::gui;

FileMenu::FileMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent),
    _openProjectAction(this, "Open"),
    _saveProjectAction(this, "Save project"),
    _saveProjectAsAction(this, "Save project as"),
    _recentProjectsMenu(this),
    _resetMenu(this),
    _importDataMenu(this),
    _pluginManagerAction(this, "Plugin manager"),
    _publishAction(this, "Publish"),
    _exitAction(this, "Exit")
{
    setTitle("File");
    setToolTip("File operations");

    addAction(&_openProjectAction);
    addAction(&_saveProjectAction);
    addAction(&_saveProjectAsAction);
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
    addAction(&_pluginManagerAction);
    addAction(&_publishAction);
    addAction(&_exitAction);

    _openProjectAction.setIcon(Application::getIconFont("FontAwesome").getIcon("folder-open"));
    _openProjectAction.setToolTip("Open project from disk");

    _saveProjectAction.setIcon(Application::getIconFont("FontAwesome").getIcon("save"));
    _saveProjectAction.setToolTip("Save project to disk");

    _saveProjectAsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("save"));
    _saveProjectAsAction.setToolTip("Save project to disk in a chosen location");

    _importDataMenu.setIcon(Application::getIconFont("FontAwesome").getIcon("file-import"));
    
    _pluginManagerAction.setIcon(Application::getIconFont("FontAwesome").getIcon("plug"));
    _pluginManagerAction.setToolTip("View loaded plugins");

    _publishAction.setIcon(Application::getIconFont("FontAwesome").getIcon("share"));
    _publishAction.setToolTip("Publish the HDPS application");

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

    //connect(&_clearDatasetsAction, &QAction::triggered, this, [this]() -> void {
    //    const auto loadedDatasets = Application::core()->requestAllDataSets();

    //    if (!loadedDatasets.empty()) {

    //        if (Application::current()->getSetting("ConfirmDataRemoval", true).toBool()) {
    //            Application::core()->removeAllDatasets();

    //            //// Ask for confirmation dialog
    //            //DataRemoveAction::ConfirmDataRemoveDialog confirmDataRemoveDialog(nullptr, "Remove all datasets", loadedDatasets);

    //            //// Show the confirm data removal dialog
    //            //confirmDataRemoveDialog.exec();

    //            //// Remove dataset and children from the core if accepted
    //            //if (confirmDataRemoveDialog.result() == 1)
    //            //    Application::core()->removeAllDatasets();
    //        }
    //    }
    //});

    connect(&_pluginManagerAction, &TriggerAction::triggered, this, [this]() -> void {
        PluginManagerDialog loadedPluginsDialog;
        loadedPluginsDialog.exec();
    });

    connect(&_exitAction, SIGNAL(triggered()), this, SLOT(close()));

    

    // Update read-only status of various actions when the main file menu is opened
    connect(this, &QMenu::aboutToShow, this, [this]() -> void {
        const auto hasDatasets = Application::core()->requestAllDataSets().size();

        _saveProjectAction.setEnabled(!Application::current()->getCurrentProjectFilePath().isEmpty());
        _saveProjectAsAction.setEnabled(hasDatasets);
        //_resetMenu.setEnabled(hasDatasets);

        _recentProjectsMenu.updateActions();

        _saveProjectAsAction.setEnabled(true);
    });
}
