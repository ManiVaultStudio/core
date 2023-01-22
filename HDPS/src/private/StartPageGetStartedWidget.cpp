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
    _createProjectFromDatasetWidget(this)
{
    auto layout = new QVBoxLayout();

    layout->addWidget(StartPageContentWidget::createHeaderLabel("Create Project From Workspace", "Create project from workspace"));
    layout->addWidget(&_createProjectFromWorkspaceWidget, 3);

    layout->addWidget(StartPageContentWidget::createHeaderLabel("Create Project From Data", "Create project from data"));
    layout->addWidget(&_createProjectFromDatasetWidget, 2);

    layout->addStretch(1);

    setLayout(layout);

    _createProjectFromWorkspaceWidget.getHierarchyWidget().setMinimumHeight(300);
    _createProjectFromWorkspaceWidget.getHierarchyWidget().setItemTypeName("Workspace");
    _createProjectFromWorkspaceWidget.getHierarchyWidget().getTreeView().verticalScrollBar()->setDisabled(true);

    _createProjectFromDatasetWidget.getHierarchyWidget().setItemTypeName("Importer");
}

void StartPageGetStartedWidget::updateActions()
{
    _createProjectFromWorkspaceWidget.getModel().reset();
    _createProjectFromDatasetWidget.getModel().reset();

    auto& fontAwesome = Application::getIconFont("FontAwesome");

    for (const auto workspaceLocation : workspaces().getWorkspaceLocations()) {
        Workspace workspace(workspaceLocation.getFilePath());

        StartPageAction fromWorkspaceStartPageAction(workspaces().getIcon(), QFileInfo(workspaceLocation.getFilePath()).baseName(), workspaceLocation.getFilePath(), workspaceLocation.getFilePath(), [workspaceLocation]() -> void {
            projects().newProject(workspaceLocation.getFilePath());
        });

        QString comments;

        fromWorkspaceStartPageAction.setComments(workspaceLocation.getTypeName());
        fromWorkspaceStartPageAction.setTags(workspace.getTagsAction().getStrings());
        fromWorkspaceStartPageAction.setPreviewImage(Workspace::getPreviewImage(workspaceLocation.getFilePath()));

        _createProjectFromWorkspaceWidget.getModel().add(fromWorkspaceStartPageAction);
    }

    StartPageAction importWorkspaceFromProjectStartPageAction(fontAwesome.getIcon("file"), "From Project", "Import workspace from project", "Pick an existing project and use its workspace", []() -> void {
        projects().newBlankProject();
        workspaces().importWorkspaceFromProjectFile();
    });

    _createProjectFromWorkspaceWidget.getModel().add(importWorkspaceFromProjectStartPageAction);

    for (auto viewPluginFactory : plugins().getPluginFactoriesByType(plugin::Type::LOADER)) {
        StartPageAction fromDataStartPageAction(viewPluginFactory->getIcon(), viewPluginFactory->getKind(), QString("Create project and import data with the %1").arg(viewPluginFactory->getKind()), "", [viewPluginFactory]() -> void {
            projects().newBlankProject();
            plugins().requestPlugin(viewPluginFactory->getKind());
        });

        _createProjectFromDatasetWidget.getModel().add(fromDataStartPageAction);
    }
    
    _createProjectFromWorkspaceWidget.createEditors();
    _createProjectFromDatasetWidget.createEditors();
}
