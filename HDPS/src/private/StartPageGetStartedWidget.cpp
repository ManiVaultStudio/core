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

    layout->addWidget(StartPageContentWidget::createHeaderLabel("Create Project From Workspace", "Create project from workspace"));
    layout->addWidget(&_createProjectFromWorkspaceWidget);

    layout->addWidget(StartPageContentWidget::createHeaderLabel("Create Project From Data", "Create project from data"));
    layout->addWidget(&_createProjectFromDatasetWidget);

    layout->addStretch(1);

    setLayout(layout);

    _createProjectFromWorkspaceWidget.getHierarchyWidget().setMinimumHeight(300);
    _createProjectFromWorkspaceWidget.getHierarchyWidget().setItemTypeName("Workspace");

    _createProjectFromDatasetWidget.getHierarchyWidget().setItemTypeName("Importer");
}

void StartPageGetStartedWidget::updateActions()
{
    _createProjectFromWorkspaceWidget.getModel().removeRows(0, _createProjectFromWorkspaceWidget.getModel().rowCount());
    _createProjectFromDatasetWidget.getModel().removeRows(0, _createProjectFromDatasetWidget.getModel().rowCount());

    auto& fontAwesome = Application::getIconFont("FontAwesome");

    for (const auto workspaceLocation : workspaces().getWorkspaceLocations()) {
        Workspace workspace(workspaceLocation.getFilePath());

        const auto icon         = workspaces().getIcon();
        const auto title        = QFileInfo(workspaceLocation.getFilePath()).baseName();
        const auto description  = workspace.getDescriptionAction().getString();
        const auto tooltip      = Workspace::getPreviewImageHtml(workspaceLocation.getFilePath(), QSize(500, 500));

        _createProjectFromWorkspaceWidget.getModel().add(icon, title, description, "", tooltip, [workspaceLocation]() -> void {
            projects().newProject(workspaceLocation.getFilePath());
        });
    }

    for (auto viewPluginFactory : plugins().getPluginFactoriesByType(plugin::Type::LOADER)) {
        _createProjectFromDatasetWidget.getModel().add(viewPluginFactory->getIcon(), viewPluginFactory->getKind(), "", "", QString("Create project and import data with the %1").arg(viewPluginFactory->getKind()), []() -> void {
            projects().newBlankProject();
        });
    }
        
}
