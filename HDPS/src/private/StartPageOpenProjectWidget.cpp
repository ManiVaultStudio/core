#include "StartPageOpenProjectWidget.h"
#include "StartPageContentWidget.h"
#include "Archiver.h"

#include <Application.h>

#include <CoreInterface.h>

#include <QDebug>

using namespace hdps;
using namespace hdps::util;

StartPageOpenProjectWidget::StartPageOpenProjectWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _openProjectWidget(this),
    _recentProjectsWidget(this),
    _recentProjectsAction(this)
{
    auto layout = new QVBoxLayout();

    layout->addWidget(StartPageContentWidget::createHeaderLabel("Open", "Open existing project"));
    layout->addWidget(&_openProjectWidget);

    layout->addWidget(StartPageContentWidget::createHeaderLabel("Recent", "Recently opened project"));
    layout->addWidget(&_recentProjectsWidget, 1);

    setLayout(layout);

    _openProjectWidget.getHierarchyWidget().getFilterNameAction().setVisible(false);
    _openProjectWidget.getHierarchyWidget().getFilterGroupAction().setVisible(false);
    _openProjectWidget.getHierarchyWidget().setFixedHeight(50);
    _openProjectWidget.getHierarchyWidget().setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
 
    _recentProjectsWidget.getHierarchyWidget().setItemTypeName("Recent Project");
}

void StartPageOpenProjectWidget::updateActions()
{
    auto& fontAwesome = Application::getIconFont("FontAwesome");

    _openProjectWidget.getModel().reset();
    _openProjectWidget.getModel().add(fontAwesome.getIcon("file"), "Open project", "Open an existing project", "", "Browse to an existing project and open it", []() -> void {
        projects().openProject();
    });

    _recentProjectsAction.initialize("Manager/Project/Recent", "Project", "Ctrl", Application::getIconFont("FontAwesome").getIcon("file"));

    _recentProjectsWidget.getModel().reset();

    for (const auto& recentFile : _recentProjectsAction.getRecentFiles()) {
        const auto recentFilePath   = recentFile.getFilePath();
        const auto icon             = fontAwesome.getIcon("file");
        const auto title            = QFileInfo(recentFilePath).baseName();
        const auto description      = recentFilePath;
        const auto comments         = recentFile.getDateTime().toString("dd/MM/yyyy hh:mm");
        const auto tooltip          = "";
        
        const auto clickedCallback = [recentFilePath]() -> void {
            projects().openProject(recentFilePath);
        };

        const auto tooltipCallback = [recentFilePath]() -> QString {
            Archiver archiver;

            const QString workspaceFile("workspace.hws");

            QTemporaryDir temporaryDirectory;

            const auto temporaryDirectoryPath = temporaryDirectory.path();

            QFileInfo workspaceFileInfo(temporaryDirectoryPath, workspaceFile);

            archiver.extractSingleFile(recentFilePath, workspaceFile, workspaceFileInfo.absoluteFilePath());

            if (workspaceFileInfo.exists())
                return Workspace::getPreviewImageHtml(workspaceFileInfo.absoluteFilePath());

            return "";
        };

        _recentProjectsWidget.getModel().add(icon, title, description, comments, tooltip, clickedCallback, tooltipCallback);
    }
        
}
