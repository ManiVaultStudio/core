// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <models/ProjectsTreeModel.h>
#include <models/ProjectsFilterModel.h>
#include <models/ProjectsModelProject.h>

#include <widgets/HierarchyWidget.h>

#include <actions/TriggerAction.h>

#include <Task.h>

#include <QDialog>

/**
 * Startup project selector dialog
 * 
 * Class for selecting a project out of two or more startup projects.
 * 
 * @author Thomas Kroes
 */
class StartupProjectSelectorDialog : public QDialog
{
public:

    /**
     * Construct with \p projectsTreeModel and a pointer to \p parent widget
     * @param projectsTreeModel Model containing the projects to select from
     * @param projectsFilterModel Model for sorting and filtering the projects
     * @param parent Pointer to parent widget
     */
    StartupProjectSelectorDialog(mv::ProjectsTreeModel& projectsTreeModel, mv::ProjectsFilterModel& projectsFilterModel, QWidget* parent = nullptr);

    /** Get preferred size */
    QSize sizeHint() const override {
        return { 800, 400 };
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

    /**
     * Get index of the selected startup project
     * @return Index of the selected startup project (-1 if no selection)
     */
    std::int32_t getSelectedStartupProjectIndex();

    /**
     * Get the selected startup project
     * @return Pointer to the selected startup project, or nullptr if no project is selected
     */
    mv::util::ProjectsModelProject* getSelectedStartupProject() const;

private:
    mv::ProjectsTreeModel&      _projectsTreeModel;     /** Projects tree model */
    mv::ProjectsFilterModel&    _projectsFilterModel;   /** Sorting and filtering model for the projects model */
    mv::gui::HierarchyWidget    _hierarchyWidget;       /** Widget for displaying the loaded plugins */
    mv::Task                    _projectDownloadTask;   /** Task for tracking project download progress */
    mv::gui::TriggerAction      _loadAction;            /** Load the selected project */
    mv::gui::TriggerAction      _quitAction;            /** Exit the dialog and don't load a project */
};
