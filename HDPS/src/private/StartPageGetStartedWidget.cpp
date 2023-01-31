#include "StartPageGetStartedWidget.h"
#include "StartPageContentWidget.h"

#include <Application.h>

#include <CoreInterface.h>

#include <QDebug>
#include <QScrollBar>

using namespace hdps;
using namespace hdps::gui;

StartPageGetStartedWidget::StartPageGetStartedWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _createProjectFromWorkspaceWidget(this),
    _createProjectFromDatasetWidget(this),
    _instructionVideosWidget(this),
    _workspaceLocationTypeAction(this, "Workspace location type"),
    _workspaceLocationTypesModel(this),
    _recentWorkspacesAction(this),
    _recentProjectsAction(this)
{
    auto layout = new QVBoxLayout();

    layout->addWidget(StartPageContentWidget::createHeaderLabel("Create Project From Workspace", "Create project from workspace"));
    layout->addWidget(&_createProjectFromWorkspaceWidget, 3);

    layout->addWidget(StartPageContentWidget::createHeaderLabel("Create Project From Data", "Create project from data"));
    layout->addWidget(&_createProjectFromDatasetWidget, 3);

    layout->addWidget(StartPageContentWidget::createHeaderLabel("Instruction Videos", "Watch instructional videos that learn how to work with the software"));
    layout->addWidget(&_instructionVideosWidget, 1);

    layout->addStretch(1);

    setLayout(layout);

    _createProjectFromWorkspaceWidget.getHierarchyWidget().setMinimumHeight(300);
    _createProjectFromWorkspaceWidget.getHierarchyWidget().setItemTypeName("Item");
    _createProjectFromWorkspaceWidget.getHierarchyWidget().getTreeView().verticalScrollBar()->setDisabled(true);

    _instructionVideosWidget.getHierarchyWidget().setWindowIcon(Application::getIconFont("FontAwesome").getIcon("video"));
    //_instructionVideosWidget.getHierarchyWidget().getFilterNameAction().setVisible(false);
    //_instructionVideosWidget.getHierarchyWidget().getFilterGroupAction().setVisible(false);
    //_instructionVideosWidget.getHierarchyWidget().setFixedHeight(86);
    //_instructionVideosWidget.getHierarchyWidget().setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    _createProjectFromDatasetWidget.getHierarchyWidget().setItemTypeName("Importer");
    _instructionVideosWidget.getHierarchyWidget().setItemTypeName("Instruction video");

    _workspaceLocationTypesModel.appendRow(new QStandardItem(hdps::Application::getIconFont("FontAwesome").getIcon("industry"), "Built-in Workspace"));
    _workspaceLocationTypesModel.appendRow(new QStandardItem(hdps::Application::getIconFont("FontAwesome").getIcon("clock"), "Recent Workspace"));
    _workspaceLocationTypesModel.appendRow(new QStandardItem(hdps::Application::getIconFont("FontAwesome").getIcon("clock"), "Recent Project"));
    
    _workspaceLocationTypeAction.setCustomModel(&_workspaceLocationTypesModel);
    _workspaceLocationTypeAction.setCurrentIndex(static_cast<std::int32_t>(FromWorkspaceType::BuiltIn));

    connect(&_workspaceLocationTypeAction, &OptionAction::currentIndexChanged, this, &StartPageGetStartedWidget::updateCreateProjectFromWorkspaceActions);

    _createProjectFromWorkspaceWidget.getHierarchyWidget().getToolbarLayout().addWidget(_workspaceLocationTypeAction.createWidget(this));

    _recentWorkspacesAction.initialize("Manager/Workspace/Recent", "Workspace", "Ctrl+Alt", Application::getIconFont("FontAwesome").getIcon("clock"));
    _recentProjectsAction.initialize("Manager/Project/Recent", "Project", "Ctrl", Application::getIconFont("FontAwesome").getIcon("clock"));
}

void StartPageGetStartedWidget::updateActions()
{
    updateCreateProjectFromWorkspaceActions();
    updateCreateProjectFromDatasetActions();
}

void StartPageGetStartedWidget::updateCreateProjectFromWorkspaceActions()
{
    _createProjectFromWorkspaceWidget.getModel().reset();
    _createProjectFromWorkspaceWidget.getHierarchyWidget().setItemTypeName(_workspaceLocationTypeAction.getCurrentText());

    auto& fontAwesome = Application::getIconFont("FontAwesome");

    switch (static_cast<FromWorkspaceType>(_workspaceLocationTypeAction.getCurrentIndex()))
    {
        case FromWorkspaceType::BuiltIn:
        {
            for (const auto workspaceLocation : workspaces().getWorkspaceLocations(WorkspaceLocation::Types(WorkspaceLocation::Type::BuiltIn))) {
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

            break;
        }

        case FromWorkspaceType::RecentWorkspace:
        {
            for (const auto& recentWorkspace : _recentWorkspacesAction.getRecentFiles()) {
                const auto recentFilePath = recentWorkspace.getFilePath();

                Workspace workspace(recentWorkspace.getFilePath());

                StartPageAction recentWorkspaceStartPageAction(workspaces().getIcon(), QFileInfo(recentFilePath).baseName(), QString("Create project from %1.hws").arg(QFileInfo(recentFilePath).baseName()), workspace.getDescriptionAction().getString(), "", [recentFilePath]() -> void {
                    projects().newProject(recentFilePath);
                });

                recentWorkspaceStartPageAction.setComments(workspace.getCommentsAction().getString());
                recentWorkspaceStartPageAction.setTags(workspace.getTagsAction().getStrings());
                recentWorkspaceStartPageAction.setMetaData(recentWorkspace.getDateTime().toString("dd/MM/yyyy hh:mm"));
                recentWorkspaceStartPageAction.setPreviewImage(workspace.getPreviewImage(recentFilePath));

                _createProjectFromWorkspaceWidget.getModel().add(recentWorkspaceStartPageAction);
            }

            break;
        }

        case FromWorkspaceType::RecentProject:
        {
            for (const auto& recentFile : _recentProjectsAction.getRecentFiles()) {
                const auto recentFilePath = recentFile.getFilePath();

                QTemporaryDir temporaryDir;

                const auto recentProjectJsonFilePath = projects().extractProjectFileFromHdpsFile(recentFilePath, temporaryDir);

                Project project(recentProjectJsonFilePath, true);

                StartPageAction recentProjectStartPageAction(fontAwesome.getIcon("clock"), QFileInfo(recentFilePath).baseName(), QString("Replicate workspace from %1.hdps in new project").arg(QFileInfo(recentFilePath).baseName()), project.getDescriptionAction().getString(), "", [recentFilePath]() -> void {
                    projects().newBlankProject();
                    workspaces().importWorkspaceFromProjectFile(recentFilePath);
                });

                recentProjectStartPageAction.setComments(project.getCommentsAction().getString());
                recentProjectStartPageAction.setTags(project.getTagsAction().getStrings());
                recentProjectStartPageAction.setMetaData(recentFile.getDateTime().toString("dd/MM/yyyy hh:mm"));
                recentProjectStartPageAction.setPreviewImage(projects().getPreviewImage(recentFilePath));
                recentProjectStartPageAction.setContributors(project.getContributorsAction().getStrings());

                _createProjectFromWorkspaceWidget.getModel().add(recentProjectStartPageAction);
            }

            break;
        }

        default:
            break;
    }
}

void StartPageGetStartedWidget::updateCreateProjectFromDatasetActions()
{
    _createProjectFromDatasetWidget.getModel().reset();

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
