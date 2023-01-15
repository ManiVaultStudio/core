#include "StartPageGetStartedWidget.h"
#include "StartPageContentWidget.h"

#include <Application.h>

#include <CoreInterface.h>

#include <QDebug>

using namespace hdps;

StartPageGetStartedWidget::StartPageGetStartedWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _createProjectFromWorkspaceWidget(this),
    _createProjectFromDatasetWidget(this)
{
    auto layout = new QVBoxLayout();

    layout->addWidget(StartPageContentWidget::createHeaderLabel("Create From Workspace", "Create project from workspace"));
    layout->addWidget(&_createProjectFromWorkspaceWidget);

    layout->addWidget(StartPageContentWidget::createHeaderLabel("Create From Data", "Create project from data"));
    layout->addWidget(&_createProjectFromDatasetWidget);

    layout->addStretch(1);

    setLayout(layout);

    _createProjectFromWorkspaceWidget.getHierarchyWidget().setMinimumHeight(300);
    _createProjectFromWorkspaceWidget.getHierarchyWidget().setItemTypeName("Workspace");

    _createProjectFromDatasetWidget.getHierarchyWidget().setItemTypeName("Importer");
}

void StartPageGetStartedWidget::updateActions()
{
    auto& fontAwesome = Application::getIconFont("FontAwesome");

    for (const auto workspaceLocation : workspaces().getWorkspaceLocations()) {
        Workspace workspace(workspaceLocation.getFilePath());

        const auto workspaceName = QFileInfo(workspaceLocation.getFilePath()).baseName();

        _createProjectFromWorkspaceWidget.getModel().add(workspaces().getIcon(), workspaceName, workspace.getDescriptionAction().getString(), "", QString("Create project from %1 workspace").arg(workspaceName), [workspaceLocation]() -> void {
            projects().newProject(workspaceLocation.getFilePath());
        });
    }

    for (auto viewPluginFactory : plugins().getPluginFactoriesByType(plugin::Type::LOADER)) {
        _createProjectFromDatasetWidget.getModel().add(viewPluginFactory->getIcon(), viewPluginFactory->getKind(), "", "", QString("Create project and import data with the %1").arg(viewPluginFactory->getKind()), []() -> void {
            projects().newBlankProject();
        });
    }
        
}
