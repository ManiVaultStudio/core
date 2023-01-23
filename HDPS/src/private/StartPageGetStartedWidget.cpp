#include "StartPageGetStartedWidget.h"
#include "StartPageContentWidget.h"

#include <Application.h>

#include <CoreInterface.h>

#include <QDebug>
#include <QScrollBar>

using namespace hdps;

StartPageGetStartedWidget::StartPageGetStartedWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _createProjectFromWorkspaceWidget(this),
    _createProjectFromDatasetWidget(this),
    _instructionVideosWidget(this)
{
    auto layout = new QVBoxLayout();

    layout->addWidget(StartPageContentWidget::createHeaderLabel("Create Project From Workspace", "Create project from workspace"));
    layout->addWidget(&_createProjectFromWorkspaceWidget, 2);

    layout->addWidget(StartPageContentWidget::createHeaderLabel("Create Project From Data", "Create project from data"));
    layout->addWidget(&_createProjectFromDatasetWidget, 2);

    layout->addWidget(StartPageContentWidget::createHeaderLabel("Instruction Videos", "Watch instructional videos that learn how to work with the software"));
    layout->addWidget(&_instructionVideosWidget, 1);

    layout->addStretch(1);

    setLayout(layout);

    _createProjectFromWorkspaceWidget.getHierarchyWidget().setMinimumHeight(300);
    _createProjectFromWorkspaceWidget.getHierarchyWidget().setItemTypeName("Workspace");
    _createProjectFromWorkspaceWidget.getHierarchyWidget().getTreeView().verticalScrollBar()->setDisabled(true);

    _instructionVideosWidget.getHierarchyWidget().setWindowIcon(Application::getIconFont("FontAwesome").getIcon("video"));
    //_instructionVideosWidget.getHierarchyWidget().getFilterNameAction().setVisible(false);
    //_instructionVideosWidget.getHierarchyWidget().getFilterGroupAction().setVisible(false);
    //_instructionVideosWidget.getHierarchyWidget().setFixedHeight(86);
    //_instructionVideosWidget.getHierarchyWidget().setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    _createProjectFromDatasetWidget.getHierarchyWidget().setItemTypeName("Importer");
    _instructionVideosWidget.getHierarchyWidget().setItemTypeName("Instruction video");
}

void StartPageGetStartedWidget::updateActions()
{
    _createProjectFromWorkspaceWidget.getModel().reset();
    _createProjectFromDatasetWidget.getModel().reset();

    auto& fontAwesome = Application::getIconFont("FontAwesome");

    for (const auto workspaceLocation : workspaces().getWorkspaceLocations(WorkspaceLocation::Type::BuiltIn)) {
        Workspace workspace(workspaceLocation.getFilePath());

        StartPageAction fromWorkspaceStartPageAction(workspaces().getIcon(), QFileInfo(workspaceLocation.getFilePath()).baseName(), workspaceLocation.getFilePath(), workspace.getDescriptionAction().getString(), workspaceLocation.getFilePath(), [workspaceLocation]() -> void {
            projects().newProject(workspaceLocation.getFilePath());
        });

        fromWorkspaceStartPageAction.setComments(workspace.getCommentsAction().getString());
        fromWorkspaceStartPageAction.setTags(workspace.getTagsAction().getStrings());
        fromWorkspaceStartPageAction.setMetaData(workspaceLocation.getTypeName());
        fromWorkspaceStartPageAction.setPreviewImage(Workspace::getPreviewImage(workspaceLocation.getFilePath()));

        _createProjectFromWorkspaceWidget.getModel().add(fromWorkspaceStartPageAction);
    }

    /*
    StartPageAction importWorkspaceFromProjectStartPageAction(fontAwesome.getIcon("file"), "From Project", "Import workspace from project", "Import workspace from project", "Pick an existing project and use its workspace", []() -> void {
        projects().newBlankProject();
        workspaces().importWorkspaceFromProjectFile();
    });

    _createProjectFromWorkspaceWidget.getModel().add(importWorkspaceFromProjectStartPageAction);
    */

    for (auto viewPluginFactory : plugins().getPluginFactoriesByType(plugin::Type::LOADER)) {
        const auto subtitle = QString("Import data into new project with %1").arg(viewPluginFactory->getKind());

        StartPageAction fromDataStartPageAction(viewPluginFactory->getIcon(), viewPluginFactory->getKind(), subtitle, subtitle, "", [viewPluginFactory]() -> void {
            projects().newBlankProject();
            plugins().requestPlugin(viewPluginFactory->getKind());
        });

        fromDataStartPageAction.setSubtitle(subtitle);
        fromDataStartPageAction.setComments(QString("Create a new project and import data into it with the %1").arg(viewPluginFactory->getKind()));

        _createProjectFromDatasetWidget.getModel().add(fromDataStartPageAction);
    }
}
