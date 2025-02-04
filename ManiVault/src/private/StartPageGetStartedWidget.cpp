// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartPageGetStartedWidget.h"
#include "StartPageContentWidget.h"

#include "PageAction.h"

#include <Application.h>
#include <ProjectMetaAction.h>

#include <util/Serialization.h>

#include <CoreInterface.h>

#include <QDebug>
#include <QDesktopServices>
#include <QScrollBar>

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

StartPageGetStartedWidget::StartPageGetStartedWidget(StartPageContentWidget* startPageContentWidget) :
    QWidget(startPageContentWidget),
    _startPageContentWidget(startPageContentWidget),
    _createProjectFromWorkspaceWidget(this, "Project From Workspace"),
    _createProjectFromDatasetWidget(this, "Project From Data"),
    _tutorialsWidget(this, { "GettingStarted" }),
    _workspaceLocationTypeAction(this, "Workspace location type"),
    _workspaceLocationTypesModel(this),
    _recentWorkspacesAction(this, mv::workspaces().getSettingsPrefix() + "RecentWorkspaces"),
    _recentProjectsAction(this, mv::projects().getSettingsPrefix() + "RecentProjects")
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(&_createProjectFromWorkspaceWidget, 3);
    layout->addWidget(&_createProjectFromDatasetWidget, 3);
    layout->addWidget(&_tutorialsWidget, 3);

    setLayout(layout);

    _createProjectFromWorkspaceWidget.getHierarchyWidget().getFilterColumnAction().setCurrentText("Title");
    _createProjectFromDatasetWidget.getHierarchyWidget().getFilterColumnAction().setCurrentText("Title");

    _createProjectFromWorkspaceWidget.getHierarchyWidget().setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _createProjectFromWorkspaceWidget.getHierarchyWidget().setItemTypeName("Item");
    _createProjectFromWorkspaceWidget.getHierarchyWidget().getTreeView().verticalScrollBar()->setDisabled(true);

    _createProjectFromDatasetWidget.getHierarchyWidget().setItemTypeName("Importer");

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
        _tutorialsWidget.setVisible(_startPageContentWidget->getToggleTutorialsAction().isChecked());
    };

    connect(&_startPageContentWidget->getToggleProjectFromWorkspaceAction(), &ToggleAction::toggled, this, toggleViews);
    connect(&_startPageContentWidget->getToggleProjectFromDataAction(), &ToggleAction::toggled, this, toggleViews);
    connect(&_startPageContentWidget->getToggleTutorialsAction(), &ToggleAction::toggled, this, toggleViews);

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

                PageAction fromWorkspacePageAction(workspaces().getIcon(), QFileInfo(workspaceLocation.getFilePath()).baseName(), workspaceLocation.getFilePath(), workspace.getDescriptionAction().getString(), workspaceLocation.getFilePath(), [workspaceLocation]() -> void {
                    projects().newProject(workspaceLocation.getFilePath());
                });

                fromWorkspacePageAction.setComments(workspace.getCommentsAction().getString());
                fromWorkspacePageAction.setTags(workspace.getTagsAction().getStrings());
                fromWorkspacePageAction.setMetaData(workspaceLocation.getTypeName());
                fromWorkspacePageAction.setPreviewImage(projects().getWorkspacePreview(workspaceLocation.getFilePath()));

                _createProjectFromWorkspaceWidget.getModel().add(fromWorkspacePageAction);
            }

            break;
        }

        case FromWorkspaceType::RecentWorkspace:
        {
            for (const auto& recentWorkspace : _recentWorkspacesAction.getRecentFiles()) {
                const auto recentFilePath = recentWorkspace.getFilePath();

                Workspace workspace(recentWorkspace.getFilePath());

                PageAction recentWorkspacePageAction(workspaces().getIcon(), QFileInfo(recentFilePath).baseName(), QString("Create project from %1.json").arg(QFileInfo(recentFilePath).baseName()), workspace.getDescriptionAction().getString(), "", [recentFilePath]() -> void {
                    projects().newProject(recentFilePath);
                });

                recentWorkspacePageAction.setComments(workspace.getCommentsAction().getString());
                recentWorkspacePageAction.setTags(workspace.getTagsAction().getStrings());
                recentWorkspacePageAction.setMetaData(recentWorkspace.getDateTime().toString("dd/MM/yyyy hh:mm"));
                //recentWorkspacePageAction.setPreviewImage(workspace.getPreviewImage(recentFilePath));

                _createProjectFromWorkspaceWidget.getModel().add(recentWorkspacePageAction);
            }

            break;
        }

        case FromWorkspaceType::RecentProject:
        {
            for (const auto& recentFile : _recentProjectsAction.getRecentFiles()) {
                const auto recentFilePath = recentFile.getFilePath();

                const auto projectMeta = Project::getProjectMetaActionFromProjectFilePath(recentFilePath);
                
                if (projectMeta.isNull()) {
                    PageAction recentProjectPageAction(fontAwesome.getIcon("clock"), QFileInfo(recentFilePath).baseName(), QString("Replicate workspace from %1.mv in new project").arg(QFileInfo(recentFilePath).baseName()), recentFilePath, "", [recentFilePath]() -> void {
                        projects().newBlankProject();
                        workspaces().importWorkspaceFromProjectFile(recentFilePath);
                    });
                
                    _createProjectFromWorkspaceWidget.getModel().add(recentProjectPageAction);
                } else {
                    PageAction recentProjectPageAction(fontAwesome.getIcon("clock"), QFileInfo(recentFilePath).baseName(), QString("Replicate workspace from %1.mv in new project").arg(QFileInfo(recentFilePath).baseName()), projectMeta->getDescriptionAction().getString(), "", [recentFilePath]() -> void {
                        projects().newBlankProject();
                        workspaces().importWorkspaceFromProjectFile(recentFilePath);
                    });
                
                    recentProjectPageAction.setComments(projectMeta->getCommentsAction().getString());
                    recentProjectPageAction.setTags(projectMeta->getTagsAction().getStrings());
                    recentProjectPageAction.setMetaData(recentFile.getDateTime().toString("dd/MM/yyyy hh:mm"));
                    recentProjectPageAction.setPreviewImage(projects().getWorkspacePreview(recentFilePath));
                    recentProjectPageAction.setContributors(projectMeta->getContributorsAction().getStrings());
                
                    _createProjectFromWorkspaceWidget.getModel().add(recentProjectPageAction);
                }
            }

            break;
        }
    }
}

void StartPageGetStartedWidget::updateCreateProjectFromDatasetActions()
{
    _createProjectFromDatasetWidget.getModel().reset();

    for (auto viewPluginFactory : plugins().getPluginFactoriesByType(plugin::Type::LOADER)) {
        const auto subtitle = QString("Import data into new project with %1").arg(viewPluginFactory->getKind());

        PageAction fromDataPageAction(viewPluginFactory->getIcon(), viewPluginFactory->getKind(), subtitle, subtitle, "", [viewPluginFactory]() -> void {
            projects().newProject(Qt::AlignRight);
            plugins().requestPlugin(viewPluginFactory->getKind());
        });

        fromDataPageAction.setSubtitle(subtitle);
        fromDataPageAction.setComments(QString("Create a new project and import data into it with the %1").arg(viewPluginFactory->getKind()));

        _createProjectFromDatasetWidget.getModel().add(fromDataPageAction);
    }
}


