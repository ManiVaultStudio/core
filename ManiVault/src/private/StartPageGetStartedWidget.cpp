// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartPageGetStartedWidget.h"
#include "StartPageContentWidget.h"

#include "PageAction.h"

#include <Application.h>
#include <ProjectMetaAction.h>

#include <util/Serialization.h>
#include <util/StyledIcon.h>

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
    _tutorialsWidget(this),
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

    _workspaceLocationTypesModel.appendRow(new QStandardItem(StyledIcon("industry"), "Built-in Workspace"));
    _workspaceLocationTypesModel.appendRow(new QStandardItem(StyledIcon("clock"), "Recent Workspace"));
    _workspaceLocationTypesModel.appendRow(new QStandardItem(StyledIcon("clock"), "Recent Project"));
    
    _workspaceLocationTypeAction.setCustomModel(&_workspaceLocationTypesModel);
    _workspaceLocationTypeAction.setCurrentIndex(static_cast<std::int32_t>(FromWorkspaceType::BuiltIn));

    connect(&_workspaceLocationTypeAction, &OptionAction::currentIndexChanged, this, &StartPageGetStartedWidget::updateCreateProjectFromWorkspaceActions);

    _createProjectFromWorkspaceWidget.getHierarchyWidget().getToolbarAction().addAction(&_workspaceLocationTypeAction);

    _recentWorkspacesAction.initialize("Workspace", "Ctrl+Alt");
    _recentProjectsAction.initialize("Project", "Ctrl");

    auto& startPageConfigurationAction = Application::current()->getConfigurationAction().getStartPageConfigurationAction();

    const auto toggleViews = [this, &startPageConfigurationAction]() -> void {
        _createProjectFromWorkspaceWidget.setVisible(startPageConfigurationAction.getToggleProjectFromWorkspaceAction().isChecked());
        _createProjectFromDatasetWidget.setVisible(startPageConfigurationAction.getToggleProjectFromDataAction().isChecked());
        _tutorialsWidget.setVisible(startPageConfigurationAction.getToggleTutorialsAction().isChecked());
    };

    connect(&startPageConfigurationAction.getToggleProjectFromWorkspaceAction(), &ToggleAction::toggled, this, toggleViews);
    connect(&startPageConfigurationAction.getToggleProjectFromDataAction(), &ToggleAction::toggled, this, toggleViews);
    connect(&startPageConfigurationAction.getToggleTutorialsAction(), &ToggleAction::toggled, this, toggleViews);

    toggleViews();
}

void StartPageGetStartedWidget::updateActions()
{
    updateCreateProjectFromWorkspaceActions();
    updateCreateProjectFromDatasetActions();
}

PageTutorialsWidget& StartPageGetStartedWidget::getTutorialsWidget()
{
    return _tutorialsWidget;
}

void StartPageGetStartedWidget::updateCreateProjectFromWorkspaceActions()
{
    _createProjectFromWorkspaceWidget.getModel().reset();
    _createProjectFromWorkspaceWidget.getHierarchyWidget().setItemTypeName(_workspaceLocationTypeAction.getCurrentText());

    switch (static_cast<FromWorkspaceType>(_workspaceLocationTypeAction.getCurrentIndex()))
    {
        case FromWorkspaceType::BuiltIn:
        {
            for (const auto workspaceLocation : workspaces().getWorkspaceLocations(WorkspaceLocation::Types(WorkspaceLocation::Type::BuiltIn))) {
                Workspace workspace(workspaceLocation.getFilePath());

                auto fromWorkspacePageAction = std::make_shared<PageAction>(workspaces().getIcon(), QFileInfo(workspaceLocation.getFilePath()).baseName(), workspaceLocation.getFilePath(), workspace.getDescriptionAction().getString(), [workspaceLocation]() -> void {
                    projects().newProject(workspaceLocation.getFilePath());
                });

                if (const auto comments = workspace.getCommentsAction().getString(); !comments.isEmpty())
                    fromWorkspacePageAction->createSubAction<CommentsPageSubAction>(comments);

                if (const auto tags = workspace.getTagsAction().getStrings(); !tags.isEmpty())
                    fromWorkspacePageAction->createSubAction<TagsPageSubAction>(tags);

                fromWorkspacePageAction->setMetaData(workspaceLocation.getTypeName());
                //fromWorkspacePageAction->setPreviewImage(projects().getWorkspacePreview(workspaceLocation.getFilePath()));

                _createProjectFromWorkspaceWidget.getModel().add(fromWorkspacePageAction);
            }

            break;
        }

        case FromWorkspaceType::RecentWorkspace:
        {
            for (const auto& recentWorkspace : _recentWorkspacesAction.getRecentFiles()) {
                const auto recentFilePath = recentWorkspace.getFilePath();

                Workspace workspace(recentWorkspace.getFilePath());

                auto recentWorkspacePageAction = std::make_shared<PageAction>(workspaces().getIcon(), QFileInfo(recentFilePath).baseName(), QString("Create project from %1.json").arg(QFileInfo(recentFilePath).baseName()), workspace.getDescriptionAction().getString(), [recentFilePath]() -> void {
                    projects().newProject(recentFilePath);
                });

                if (const auto comments = workspace.getCommentsAction().getString(); !comments.isEmpty())
                    recentWorkspacePageAction->createSubAction<CommentsPageSubAction>(comments);

                if (const auto tags = workspace.getTagsAction().getStrings(); !tags.isEmpty())
                    recentWorkspacePageAction->createSubAction<TagsPageSubAction>(tags);

                recentWorkspacePageAction->setMetaData(recentWorkspace.getDateTime().toString("dd/MM/yyyy hh:mm"));
                //recentWorkspacePageAction->setPreviewImage(workspace.getPreviewImage(recentFilePath));

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
                    auto recentProjectPageAction = std::make_shared<PageAction>(StyledIcon("clock"), QFileInfo(recentFilePath).baseName(), QString("Replicate workspace from %1.mv in new project").arg(QFileInfo(recentFilePath).baseName()), recentFilePath, [recentFilePath]() -> void {
                        projects().newBlankProject();
                        workspaces().importWorkspaceFromProjectFile(recentFilePath);
                    });
                
                    _createProjectFromWorkspaceWidget.getModel().add(recentProjectPageAction);
                } else {
                    auto recentProjectPageAction = std::make_shared<PageAction>(StyledIcon("clock"), QFileInfo(recentFilePath).baseName(), QString("Replicate workspace from %1.mv in new project").arg(QFileInfo(recentFilePath).baseName()), projectMeta->getDescriptionAction().getString(), [recentFilePath]() -> void {
                        projects().newBlankProject();
                        workspaces().importWorkspaceFromProjectFile(recentFilePath);
                    });

                    if (const auto comments = projectMeta->getCommentsAction().getString(); !comments.isEmpty())
                        recentProjectPageAction->createSubAction<CommentsPageSubAction>(comments);

                    if (const auto tags = projectMeta->getTagsAction().getStrings(); !tags.isEmpty())
                        recentProjectPageAction->createSubAction<TagsPageSubAction>(tags);

                    if (const auto contributors = projectMeta->getContributorsAction().getStrings(); !contributors.isEmpty())
                        recentProjectPageAction->createSubAction<ContributorsPageSubAction>(contributors);

                    recentProjectPageAction->setMetaData(recentFile.getDateTime().toString("dd/MM/yyyy hh:mm"));
                    //recentProjectPageAction->setPreviewImage(projects().getWorkspacePreview(recentFilePath));
                
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

        auto fromDataPageAction = std::make_shared<PageAction>(StyledIcon(viewPluginFactory->icon()), viewPluginFactory->getKind(), subtitle, "", [viewPluginFactory]() -> void {
            projects().newProject(Qt::AlignRight);
            plugins().requestPlugin(viewPluginFactory->getKind());
        });

        fromDataPageAction->setSubtitle(subtitle);
        fromDataPageAction->createSubAction<CommentsPageSubAction>(QString("Create a new project and import data into it with the %1").arg(viewPluginFactory->getKind()));

        _createProjectFromDatasetWidget.getModel().add(fromDataPageAction);
    }
}

