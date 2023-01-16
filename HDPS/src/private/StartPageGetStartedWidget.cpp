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
    layout->addWidget(&_createProjectFromWorkspaceWidget);

    layout->addWidget(StartPageContentWidget::createHeaderLabel("Create Project From Data", "Create project from data"));
    layout->addWidget(&_createProjectFromDatasetWidget);

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

        const auto icon         = workspaces().getIcon();
        const auto title        = QFileInfo(workspaceLocation.getFilePath()).baseName();
        const auto description  = workspaceLocation.getFilePath();
        const auto comments     = "";
        const auto tags         = workspace.getTagsAction().getStrings();
        const auto tooltip      = Workspace::getPreviewImageHtml(workspaceLocation.getFilePath());

        _createProjectFromWorkspaceWidget.getModel().add(icon, title, description, comments, tags, tooltip, [workspaceLocation]() -> void {
            projects().newProject(workspaceLocation.getFilePath());
        });
    }

    for (auto viewPluginFactory : plugins().getPluginFactoriesByType(plugin::Type::LOADER)) {
        const auto icon         = viewPluginFactory->getIcon();
        const auto title        = viewPluginFactory->getKind();
        const auto description  = "";
        const auto comments     = "";
        const auto tags         = QStringList();
        const auto tooltip      = QString("Create project and import data with the %1").arg(viewPluginFactory->getKind());

        _createProjectFromDatasetWidget.getModel().add(icon, title, description, comments, tags, tooltip, [viewPluginFactory]() -> void {
            projects().newBlankProject();
            plugins().requestPlugin(viewPluginFactory->getKind());
        });
    }
        
}
