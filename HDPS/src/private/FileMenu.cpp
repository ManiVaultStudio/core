#include "FileMenu.h"

#include <Application.h>
#include <CoreInterface.h>

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

    addMenu(&projects().getNewProjectMenu());
    addAction(&projects().getOpenProjectAction());
    //addAction(&projects().getImportProjectAction());
    addAction(&projects().getSaveProjectAction());
    addAction(&projects().getSaveProjectAsAction());
    addAction(&projects().getEditProjectSettingsAction());
    addMenu(projects().getRecentProjectsAction().getMenu());
    addSeparator();
    addMenu(Application::core()->getWorkspaceManager().getMenu());
    addSeparator();
    addMenu(&projects().getImportDataMenu());
    addSeparator();
    addAction(&projects().getPluginManagerAction());
    addSeparator();
    addAction(&settings().getEditSettingsAction());
    addSeparator();
    addAction(&projects().getShowStartPageAction());
    //addSeparator();
    //addAction(&_exitApplictionAction);
}
