// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "StartPageOpenProjectWidget.h"
#include "StartPageGetStartedWidget.h"

#include <actions/ToggleAction.h>
#include <actions/OptionsAction.h>

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

/**
 * Start page content widget class
 *
 * Widget class for showingall the actions.
 *
 * @author Thomas Kroes
 */
class StartPageContentWidget final : public QWidget
{
protected:

    /**
     * Construct with \p parent widget
     * @param parent Pointer to parent widget
     */
    StartPageContentWidget(QWidget* parent = nullptr);

    /** Updates the actions to reflect changes */
    void updateActions();

public:

    /**
     * Create header label
     * @param title Title of the header
     * @param tooltip Tooltip of the header
     * @return Pointer to newly created header label
     */
    static QLabel* createHeaderLabel(const QString& title, const QString& tooltip);

public: // Action getters

    const mv::gui::ToggleAction& getCompactViewAction() const { return _compactViewAction; }
    const mv::gui::ToggleAction& getToggleOpenCreateProjectAction() const { return _toggleOpenCreateProjectAction; }
    const mv::gui::ToggleAction& getToggleRecentProjectsAction() const { return _toggleRecentProjectsAction; }
    const mv::gui::ToggleAction& getToggleExampleProjectsAction() const { return _toggleExampleProjectsAction; }
    const mv::gui::ToggleAction& getToggleProjectFromWorkspaceAction() const { return _toggleProjectFromWorkspaceAction; }
    const mv::gui::ToggleAction& getToggleProjectFromDataAction() const { return _toggleProjectFromDataAction; }
    const mv::gui::ToggleAction& getToggleTutorialVideosAction() const { return _toggleTutorialVideosAction; }
        
private slots:
    
    /**
     * Update custom theme parts not caught by the system itself
     */
    void updateCustomStyle() ;

private:
    QVBoxLayout                 _mainLayout;                        /** Main layout */
    QHBoxLayout                 _collumnsLayout;                    /** Columns layout */
    QHBoxLayout                 _toolbarLayout;                     /** Toolbar layout */
    mv::gui::ToggleAction     _compactViewAction;                 /** Toggle compact view on/off */
    mv::gui::ToggleAction     _toggleOpenCreateProjectAction;     /** Toggle open and create project section */
    mv::gui::ToggleAction     _toggleRecentProjectsAction;        /** Toggle recent projects section */
    mv::gui::ToggleAction     _toggleExampleProjectsAction;       /** Toggle example projects section */
    mv::gui::ToggleAction     _toggleProjectFromWorkspaceAction;  /** Toggle project from workspace section */
    mv::gui::ToggleAction     _toggleProjectFromDataAction;       /** Toggle project from data section */
    mv::gui::ToggleAction     _toggleTutorialVideosAction;        /** Toggle tutorial videos section */
    mv::gui::GroupAction      _settingsAction;                    /** Settings action */
    StartPageOpenProjectWidget  _openProjectWidget;                 /** Left column widget for opening existing projects */
    StartPageGetStartedWidget   _getStartedWidget;                  /** Right column widget for getting started operations */

    friend class StartPageWidget;
};
