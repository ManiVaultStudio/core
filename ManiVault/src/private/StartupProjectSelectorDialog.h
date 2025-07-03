// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "StartupProjectsModel.h"
#include "StartupProjectsFilterModel.h"

#include <models/ProjectsTreeModel.h>
#include <models/ProjectsFilterModel.h>

#include <widgets/HierarchyWidget.h>

#include <actions/TriggerAction.h>

#include <QDialog>

/**
 * Startup project selector dialog class
 * 
 * Dialog class for selecting a project out of two or more startup projects
 * 
 * @author Thomas Kroes
 */
class StartupProjectSelectorDialog : public QDialog
{
public:

    /**
     * Construct with \p projectsTreeModel and a pointer to \p parent widget
     * @param projectsTreeModel Model containing the projects to select from
     * @param parent Pointer to parent widget
     */
    StartupProjectSelectorDialog(mv::ProjectsTreeModel& projectsTreeModel, QWidget* parent = nullptr);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(500, 150);
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

private:
    mv::ProjectsTreeModel&       _projectsTreeModel;    /** Projects tree model */
    mv::ProjectsFilterModel     _filterModel;           /** Sorting and filtering model for the projects model */
    mv::gui::HierarchyWidget    _hierarchyWidget;       /** Widget for displaying the loaded plugins */
    mv::gui::TriggerAction      _loadAction;            /** Load the selected project */
    mv::gui::TriggerAction      _quitAction;            /** Exit the dialog and don't load a project */
};
