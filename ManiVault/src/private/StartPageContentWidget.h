// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "PageContentWidget.h"

#include <util/Serializable.h>

#include <actions/ToggleAction.h>
#include <actions/TriggerAction.h>

#include "StartPageOpenProjectWidget.h"
#include "StartPageGetStartedWidget.h"

/**
 * Start page content widget class
 *
 * Widget class for showing all the actions.
 *
 * @author Thomas Kroes
 */
class StartPageContentWidget final : public PageContentWidget, public mv::util::Serializable
{
protected:

    /**
     * Construct with \p parent widget
     * @param parent Pointer to parent widget
     */
    StartPageContentWidget(QWidget* parent = nullptr);

    /** Updates the actions to reflect changes */
    void updateActions();

public: // Serialization

    /**
     * Load from variant map
     * @param variantMap Variant map
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save to variant map
     * @return Variant map
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    const mv::gui::ToggleAction& getCompactViewAction() const { return _compactViewAction; }
    const mv::gui::ToggleAction& getToggleOpenCreateProjectAction() const { return _toggleOpenCreateProjectAction; }
    const mv::gui::ToggleAction& getToggleProjectDatabaseAction() const { return _toggleProjectDatabaseAction; }
    const mv::gui::ToggleAction& getToggleRecentProjectsAction() const { return _toggleRecentProjectsAction; }
    const mv::gui::ToggleAction& getToggleProjectFromDataAction() const { return _toggleProjectFromDataAction; }
    const mv::gui::ToggleAction& getToggleProjectFromWorkspaceAction() const { return _toggleProjectFromWorkspaceAction; }
    const mv::gui::ToggleAction& getToggleTutorialsAction() const { return _toggleTutorialsAction; }
    const mv::gui::GroupAction& getSettingsAction() const { return _settingsAction; }
    const mv::gui::TriggerAction& getToLearningCenterAction() const { return _toLearningCenterAction; }
    mv::gui::HorizontalGroupAction& getToolbarAction() { return _toolbarAction; }

private:
    mv::gui::ToggleAction           _compactViewAction;                 /** Toggle compact view on/off */
    mv::gui::ToggleAction           _toggleOpenCreateProjectAction;     /** Toggle open and create project section */
    mv::gui::ToggleAction           _toggleProjectDatabaseAction;       /** Toggle project database section */
    mv::gui::ToggleAction           _toggleRecentProjectsAction;        /** Toggle recent projects section */
    mv::gui::ToggleAction           _toggleProjectFromDataAction;       /** Toggle project from data section */
    mv::gui::ToggleAction           _toggleProjectFromWorkspaceAction;  /** Toggle project from workspace section */
	mv::gui::ToggleAction           _toggleTutorialsAction;             /** Toggle tutorials section */
    mv::gui::GroupAction            _settingsAction;                    /** Settings action */
    mv::gui::TriggerAction          _toLearningCenterAction;            /** Trigger action for showing the learning center */
    mv::gui::HorizontalGroupAction  _toolbarAction;                     /** Bottom toolbar action */
    StartPageOpenProjectWidget      _openProjectWidget;                 /** Left column widget for opening existing projects */
    StartPageGetStartedWidget       _getStartedWidget;                  /** Right column widget for getting started operations */

    friend class StartPageWidget;
};