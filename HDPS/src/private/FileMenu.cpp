#include "FileMenu.h"

#include <Application.h>

using namespace hdps;

FileMenu::FileMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent)
{
    setTitle("File");
    setToolTip("File operations");
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
    addMenu(projectManager.getImportDataMenu());
    addSeparator();
    addAction(&projectManager.getPluginManagerAction());
    addSeparator();
    //addAction(&Application::current()->getGlobalSettingsAction());
    addSeparator();
    addAction(&projectManager.getShowStartPageAction());
    //addAction(&Application::current()->getExitAction());
}
