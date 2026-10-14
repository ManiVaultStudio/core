// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <models/ProjectsFilterModel.h>

#include <QMenu>

/**
 * Projects menu class
 * 
 * For listing projects and launching them
 * 
 * @author Thomas Kroes
 */
class ProjectsMenu : public QMenu
{
public:

    /**
     * Construct with pointer to \parent widget
     * @param parent Pointer to parent widget
     */
    ProjectsMenu(QWidget *parent = nullptr);

    /**
     * Invoked when the menu is shown
     * @param event Pointer to show event
     */
    void showEvent(QShowEvent* event) override;

private:
    
    /** Populate the menu */
    void populate ();

private:
    mv::ProjectsFilterModel     _projectsFilterModel;   /** Projects filter model for filtering projects */
};
