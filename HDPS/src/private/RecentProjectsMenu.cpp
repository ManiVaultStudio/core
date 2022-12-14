#include "RecentProjectsMenu.h"

#include <Application.h>

using namespace hdps;

RecentProjectsMenu::RecentProjectsMenu(QWidget* parent /*= nullptr*/) :
    QMenu(parent)
{
    setTitle("Recent projects...");
    setToolTip("Recently opened HDPS projects");
    setIcon(Application::getIconFont("FontAwesome").getIcon("clock"));

    clear();

    const auto recentProjects = Application::current()->getSetting("Projects/Recent", QVariantList()).toList();

    setEnabled(!recentProjects.isEmpty());

    for (const auto& recentProject : recentProjects) {
        const auto recentProjectFilePath = recentProject.toMap()["FilePath"].toString();

        if (!QFileInfo(recentProjectFilePath).exists())
            continue;

        auto recentProjectAction = new QAction(recentProjectFilePath);

        recentProjectAction->setIcon(Application::getIconFont("FontAwesome").getIcon("file"));
        recentProjectAction->setToolTip("Load " + recentProjectFilePath + "(last opened on " + recentProject.toMap()["DateTime"].toDate().toString() + ")");

        connect(recentProjectAction, &QAction::triggered, this, [recentProjectFilePath]() -> void {
            Application::current()->loadProject(recentProjectFilePath);
        });

        addAction(recentProjectAction);
    }
}
