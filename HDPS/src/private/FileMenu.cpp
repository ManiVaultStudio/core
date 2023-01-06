#include "FileMenu.h"

#include <Application.h>

using namespace hdps;
using namespace hdps::gui;

FileMenu::FileMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent),
    _exitApplictionAction(nullptr, "Exit")
{
    setTitle("File");
    setToolTip("File operations");

    _exitApplictionAction.setShortcut(QKeySequence("Alt+F4"));
    _exitApplictionAction.setShortcutContext(Qt::ApplicationShortcut);
    _exitApplictionAction.setIcon(Application::getIconFont("FontAwesome").getIcon("sign-out-alt"));
    _exitApplictionAction.setToolTip("Exit HDPS");

    connect(&_exitApplictionAction, &TriggerAction::triggered, this, []() -> void {
        Application::current()->quit();
    });
}

void FileMenu::showEvent(QShowEvent* showEvent)
{
    clear();

    auto& projectManager = Application::core()->getProjectManager();

    addAction(&projectManager.getNewProjectAction());
    addAction(&projectManager.getOpenProjectAction());
    //addAction(&projectManager.getImportProjectAction());
    addAction(&projectManager.getSaveProjectAction());
    addAction(&projectManager.getSaveProjectAsAction());
    addAction(&projectManager.getEditProjectSettingsAction());
    addMenu(projectManager.getRecentProjectsAction().getMenu());
    addSeparator();
    addMenu(Application::core()->getWorkspaceManager().getMenu());
    addSeparator();
    addMenu(projectManager.getImportDataMenu());
    addSeparator();
    addAction(&projectManager.getPluginManagerAction());
    addSeparator();
    addAction(&Application::core()->getSettingsManager().getEditSettingsAction());
    addSeparator();
    addAction(&projectManager.getShowStartPageAction());
    addSeparator();
    addAction(&_exitApplictionAction);
}
