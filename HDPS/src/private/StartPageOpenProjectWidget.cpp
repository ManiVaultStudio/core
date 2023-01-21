#include "StartPageOpenProjectWidget.h"
#include "StartPageContentWidget.h"
#include "Archiver.h"

#include <Application.h>

#include <CoreInterface.h>

#include <QDebug>
#include <QImage>

using namespace hdps;
using namespace hdps::util;

StartPageOpenProjectWidget::StartPageOpenProjectWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _openProjectWidget(this),
    _recentProjectsWidget(this),
    _exampleProjectsWidget(this),
    _recentProjectsAction(this)
{
    auto layout = new QVBoxLayout();

    layout->addWidget(StartPageContentWidget::createHeaderLabel("Open", "Open existing project"));
    layout->addWidget(&_openProjectWidget);

    layout->addWidget(StartPageContentWidget::createHeaderLabel("Recent", "Recently opened project"));
    layout->addWidget(&_recentProjectsWidget);

    layout->addWidget(StartPageContentWidget::createHeaderLabel("Examples", "Open example project"));
    layout->addWidget(&_exampleProjectsWidget);

    layout->addStretch(1);

    setLayout(layout);

    _openProjectWidget.getHierarchyWidget().getFilterNameAction().setVisible(false);
    _openProjectWidget.getHierarchyWidget().getFilterGroupAction().setVisible(false);
    _openProjectWidget.getHierarchyWidget().setFixedHeight(50);
    _openProjectWidget.getHierarchyWidget().setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
 
    _recentProjectsWidget.getHierarchyWidget().setItemTypeName("Recent Project");
    _exampleProjectsWidget.getHierarchyWidget().setItemTypeName("Example Project");
}

void StartPageOpenProjectWidget::updateActions()
{
    auto& fontAwesome = Application::getIconFont("FontAwesome");

    _openProjectWidget.getModel().reset();

    StartPageAction openProjectStartPageAction(fontAwesome.getIcon("folder-open"), "Open project", "Open an existing project", "Browse to an existing project and open it", []() -> void {
        projects().openProject();
    });

    _openProjectWidget.getModel().add(openProjectStartPageAction);

    _recentProjectsAction.initialize("Manager/Project/Recent", "Project", "Ctrl", Application::getIconFont("FontAwesome").getIcon("file"));

    _recentProjectsWidget.getModel().reset();

    StartPageAction blankProjectStartPageAction(fontAwesome.getIcon("file"), "Blank", "Create project without any plugins", "Create a blank project without any plugins", []() -> void {
        projects().newBlankProject();
    });

    _recentProjectsWidget.getModel().add(blankProjectStartPageAction);

    for (const auto& recentFile : _recentProjectsAction.getRecentFiles()) {
        const auto recentFilePath = recentFile.getFilePath();
        
        StartPageAction recentProjectStartPageAction(fontAwesome.getIcon("file"), QFileInfo(recentFilePath).baseName(), recentFilePath, "", [recentFilePath]() -> void {
            projects().openProject(recentFilePath);
        });

        recentProjectStartPageAction.setComments(recentFile.getDateTime().toString("dd/MM/yyyy hh:mm"));
        recentProjectStartPageAction.setPreviewImage(projects().getPreviewImage(recentFilePath));

        _recentProjectsWidget.getModel().add(recentProjectStartPageAction);
    }

    QStringList projectFilter("*.hdps");

    QDir directory(QString("%1/examples").arg(qApp->applicationDirPath()));

    const auto exampleProjects = directory.entryList(projectFilter);

    for (const auto& exampleProject : exampleProjects) {
        const auto exampleProjectFilePath = QString("%1/examples/%2").arg(qApp->applicationDirPath(), exampleProject);

        QTemporaryDir temporaryDir;

        const auto exampleProjectJsonFilePath = projects().extractProjectFileFromHdpsFile(exampleProjectFilePath, temporaryDir);

        Project project(exampleProjectJsonFilePath);

        StartPageAction exampleProjectStartPageAction(fontAwesome.getIcon("file-prescription"), project.getTitleAction().getString(), project.getDescriptionAction().getString(), "", [exampleProjectFilePath]() -> void {
            projects().openProject(exampleProjectFilePath);
        });

        exampleProjectStartPageAction.setPreviewImage(projects().getPreviewImage(exampleProjectFilePath));

        _exampleProjectsWidget.getModel().add(exampleProjectStartPageAction);
    }

    _recentProjectsWidget.createEditors();
    _exampleProjectsWidget.createEditors();
}
