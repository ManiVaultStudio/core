// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "PageActionsWidget.h"

#include <actions/OptionAction.h>
#include <actions/RecentFilesAction.h>

#include <QWidget>
#include <QStandardItemModel>

#include <models/LearningCenterTutorialsFilterModel.h>

class StartPageContentWidget;

/**
 * Start page get started widget class
 *
 * Widget class which contains actions related to getting started.
 *
 * @author Thomas Kroes
 */
class StartPageGetStartedWidget : public QWidget
{
public:

    /** Ways to create a new project from workspace */
    enum class FromWorkspaceType {
        BuiltIn,            /** Create project from built-in workspace */
        RecentWorkspace,    /** Create project from recent workspace */
        RecentProject       /** Replicate workspace from recent project in new project */
    };

protected:

    /**
     * Construct with \p startPageContentWidget
     * @param startPageContentWidget Pointer to start page content widget
     */
    StartPageGetStartedWidget(StartPageContentWidget* startPageContentWidget);

    /** Updates the actions to reflect changes */
    void updateActions();

private:

    /** Update actions for creating a project from workspace */
    void updateCreateProjectFromWorkspaceActions();

    /** Update actions for creating a project from dataset */
    void updateCreateProjectFromDatasetActions();

    /** Update actions for launching tutorials */
    void updateTutorialActions();

private:
    StartPageContentWidget*                     _startPageContentWidget;                /** Non-owning pointer to start page content widget */
    PageActionsWidget                           _createProjectFromWorkspaceWidget;      /** Actions widget for creating a project from workspace */
    PageActionsWidget                           _createProjectFromDatasetWidget;        /** Actions widget for creating a project from a dataset */
    mv::LearningCenterTutorialsFilterModel      _learningCenterTutorialsFilterModel;    /** For filtering learning center tutorials */
    PageActionsWidget                           _tutorialsWidget;                       /** Actions widget for launching tutorials */
    mv::gui::OptionAction                       _workspaceLocationTypeAction;           /** Action for filtering workspace location types (create project from built-in workspace or import from project) */
    QStandardItemModel                          _workspaceLocationTypesModel;           /** Input model for the above workspace location type action */
    mv::gui::RecentFilesAction                  _recentWorkspacesAction;                /** Action for recent workspaces (create project from recent workspace) */
    mv::gui::RecentFilesAction                  _recentProjectsAction;                  /** Action for recent projects (replicate workspace from recent project) */

    friend class StartPageContentWidget;
};