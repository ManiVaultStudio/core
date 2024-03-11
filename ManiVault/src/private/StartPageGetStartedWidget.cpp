// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartPageGetStartedWidget.h"
#include "StartPageContentWidget.h"

#include <Application.h>
#include <ProjectMetaAction.h>

#include <util/Serialization.h>

#include <CoreInterface.h>

#include <QDebug>
#include <QScrollBar>

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

StartPageGetStartedWidget::StartPageGetStartedWidget(StartPageContentWidget* startPageContentWidget) :
    QWidget(startPageContentWidget),
    _startPageContentWidget(startPageContentWidget),
    _createProjectFromWorkspaceWidget(this, "Project From Workspace"),
    _createProjectFromDatasetWidget(this, "Project From Data"),
    _instructionVideosWidget(this, "Instruction Videos"),
    _workspaceLocationTypeAction(this, "Workspace location type"),
    _workspaceLocationTypesModel(this),
    _recentWorkspacesAction(this, mv::workspaces().getSettingsPrefix() + "RecentWorkspaces"),
    _recentProjectsAction(this, mv::projects().getSettingsPrefix() + "RecentProjects")
{
    auto layout = new QVBoxLayout();

    layout->addWidget(&_createProjectFromWorkspaceWidget, 3);
    layout->addWidget(&_createProjectFromDatasetWidget, 3);
    layout->addWidget(&_instructionVideosWidget, 1);

    setLayout(layout);

    _createProjectFromWorkspaceWidget.getHierarchyWidget().getFilterColumnAction().setCurrentText("Title");
    _createProjectFromDatasetWidget.getHierarchyWidget().getFilterColumnAction().setCurrentText("Title");
    _instructionVideosWidget.getHierarchyWidget().getFilterColumnAction().setCurrentText("Title");

    _createProjectFromWorkspaceWidget.getHierarchyWidget().setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _createProjectFromWorkspaceWidget.getHierarchyWidget().setItemTypeName("Item");
    _createProjectFromWorkspaceWidget.getHierarchyWidget().getTreeView().verticalScrollBar()->setDisabled(true);

    _instructionVideosWidget.getHierarchyWidget().setWindowIcon(Application::getIconFont("FontAwesome").getIcon("video"));
    //_instructionVideosWidget.getHierarchyWidget().getFilterNameAction().setVisible(false);
    //_instructionVideosWidget.getHierarchyWidget().getFilterGroupAction().setVisible(false);
    //_instructionVideosWidget.getHierarchyWidget().setFixedHeight(86);
    //_instructionVideosWidget.getHierarchyWidget().setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    _createProjectFromDatasetWidget.getHierarchyWidget().setItemTypeName("Importer");
    _instructionVideosWidget.getHierarchyWidget().setItemTypeName("Instruction video");

    _workspaceLocationTypesModel.appendRow(new QStandardItem(mv::Application::getIconFont("FontAwesome").getIcon("industry"), "Built-in Workspace"));
    _workspaceLocationTypesModel.appendRow(new QStandardItem(mv::Application::getIconFont("FontAwesome").getIcon("clock"), "Recent Workspace"));
    _workspaceLocationTypesModel.appendRow(new QStandardItem(mv::Application::getIconFont("FontAwesome").getIcon("clock"), "Recent Project"));
    
    _workspaceLocationTypeAction.setCustomModel(&_workspaceLocationTypesModel);
    _workspaceLocationTypeAction.setCurrentIndex(static_cast<std::int32_t>(FromWorkspaceType::BuiltIn));

    connect(&_workspaceLocationTypeAction, &OptionAction::currentIndexChanged, this, &StartPageGetStartedWidget::updateCreateProjectFromWorkspaceActions);

    _createProjectFromWorkspaceWidget.getHierarchyWidget().getToolbarAction().addAction(&_workspaceLocationTypeAction);

    _recentWorkspacesAction.initialize("Manager/Workspace/Recent", "Workspace", "Ctrl+Alt", Application::getIconFont("FontAwesome").getIcon("clock"));
    _recentProjectsAction.initialize("Manager/Project/Recent", "Project", "Ctrl", Application::getIconFont("FontAwesome").getIcon("clock"));

    const auto toggleViews = [this]() -> void {
        _createProjectFromWorkspaceWidget.setVisible(_startPageContentWidget->getToggleProjectFromWorkspaceAction().isChecked());
        _createProjectFromDatasetWidget.setVisible(_startPageContentWidget->getToggleProjectFromDataAction().isChecked());
        _instructionVideosWidget.setVisible(_startPageContentWidget->getToggleTutorialVideosAction().isChecked());
    };

    connect(&_startPageContentWidget->getToggleProjectFromWorkspaceAction(), &ToggleAction::toggled, this, toggleViews);
    connect(&_startPageContentWidget->getToggleProjectFromDataAction(), &ToggleAction::toggled, this, toggleViews);
    connect(&_startPageContentWidget->getToggleTutorialVideosAction(), &ToggleAction::toggled, this, toggleViews);

    toggleViews();
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
                fromWorkspaceStartPageAction.setPreviewImage(projects().getWorkspacePreview(workspaceLocation.getFilePath()));

                _createProjectFromWorkspaceWidget.getModel().add(fromWorkspaceStartPageAction);
            }

            break;
        }

        case FromWorkspaceType::RecentWorkspace:
        {
            for (const auto& recentWorkspace : _recentWorkspacesAction.getRecentFiles()) {
                const auto recentFilePath = recentWorkspace.getFilePath();

                Workspace workspace(recentWorkspace.getFilePath());

                StartPageAction recentWorkspaceStartPageAction(workspaces().getIcon(), QFileInfo(recentFilePath).baseName(), QString("Create project from %1.json").arg(QFileInfo(recentFilePath).baseName()), workspace.getDescriptionAction().getString(), "", [recentFilePath]() -> void {
                    projects().newProject(recentFilePath);
                });

                recentWorkspaceStartPageAction.setComments(workspace.getCommentsAction().getString());
                recentWorkspaceStartPageAction.setTags(workspace.getTagsAction().getStrings());
                recentWorkspaceStartPageAction.setMetaData(recentWorkspace.getDateTime().toString("dd/MM/yyyy hh:mm"));
                //recentWorkspaceStartPageAction.setPreviewImage(workspace.getPreviewImage(recentFilePath));

                _createProjectFromWorkspaceWidget.getModel().add(recentWorkspaceStartPageAction);
            }

            break;
        }

        case FromWorkspaceType::RecentProject:
        {
            for (const auto& recentFile : _recentProjectsAction.getRecentFiles()) {
                const auto recentFilePath = recentFile.getFilePath();

                const auto projectMeta = Project::getProjectMetaActionFromProjectFilePath(recentFilePath);
                
                if (projectMeta.isNull()) {
                    StartPageAction recentProjectStartPageAction(fontAwesome.getIcon("clock"), QFileInfo(recentFilePath).baseName(), QString("Replicate workspace from %1.mv in new project").arg(QFileInfo(recentFilePath).baseName()), recentFilePath, "", [recentFilePath]() -> void {
                        projects().newBlankProject();
                        workspaces().importWorkspaceFromProjectFile(recentFilePath);
                    });
                
                    _createProjectFromWorkspaceWidget.getModel().add(recentProjectStartPageAction);
                } else {
                    StartPageAction recentProjectStartPageAction(fontAwesome.getIcon("clock"), QFileInfo(recentFilePath).baseName(), QString("Replicate workspace from %1.mv in new project").arg(QFileInfo(recentFilePath).baseName()), projectMeta->getDescriptionAction().getString(), "", [recentFilePath]() -> void {
                        projects().newBlankProject();
                        workspaces().importWorkspaceFromProjectFile(recentFilePath);
                    });
                
                    recentProjectStartPageAction.setComments(projectMeta->getCommentsAction().getString());
                    recentProjectStartPageAction.setTags(projectMeta->getTagsAction().getStrings());
                    recentProjectStartPageAction.setMetaData(recentFile.getDateTime().toString("dd/MM/yyyy hh:mm"));
                    recentProjectStartPageAction.setPreviewImage(projects().getWorkspacePreview(recentFilePath));
                    recentProjectStartPageAction.setContributors(projectMeta->getContributorsAction().getStrings());
                
                    _createProjectFromWorkspaceWidget.getModel().add(recentProjectStartPageAction);
                }
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
            projects().newProject(Qt::AlignRight);
            plugins().requestPlugin(viewPluginFactory->getKind());
        });

        fromDataStartPageAction.setSubtitle(subtitle);
        fromDataStartPageAction.setComments(QString("Create a new project and import data into it with the %1").arg(viewPluginFactory->getKind()));

        _createProjectFromDatasetWidget.getModel().add(fromDataStartPageAction);
    }
}
