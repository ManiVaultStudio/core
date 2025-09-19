// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "VerticalGroupAction.h"
#include "ToggleAction.h"

namespace mv::gui {

/**
 * State page configuration action class
 *
 * Bundles start page settings
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT StartPageConfigurationAction : public VerticalGroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to /p parent object and /p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE StartPageConfigurationAction(QObject* parent, const QString& title);

public: // Serialization

    /**
     * Load application configuration from variant map
     * @param variantMap Variant map representation of the application configuration
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save application configuration to variant maps
     * @return Variant representation of the application configuration
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    ToggleAction& getCompactViewAction() { return _compactViewAction; }
    ToggleAction& getToggleOpenCreateProjectAction() { return _toggleOpenCreateProjectAction; }
    ToggleAction& getToggleProjectDatabaseAction() { return _toggleProjectDatabaseAction; }
    ToggleAction& getToggleRecentProjectsAction() { return _toggleRecentProjectsAction; }
    ToggleAction& getToggleProjectFromDataAction() { return _toggleProjectFromDataAction; }
    ToggleAction& getToggleProjectFromWorkspaceAction() { return _toggleProjectFromWorkspaceAction; }
    ToggleAction& getToggleTutorialsAction() { return _toggleTutorialsAction; }

private:
    ToggleAction    _compactViewAction;                 /** Toggle compact view on/off */
    ToggleAction    _toggleOpenCreateProjectAction;     /** Toggle open and create project section */
    ToggleAction    _toggleProjectDatabaseAction;       /** Toggle project database section */
    ToggleAction    _toggleRecentProjectsAction;        /** Toggle recent projects section */
    ToggleAction    _toggleProjectFromDataAction;       /** Toggle project from data section */
    ToggleAction    _toggleProjectFromWorkspaceAction;  /** Toggle project from workspace section */
    ToggleAction    _toggleTutorialsAction;             /** Toggle tutorials section */
};

}

Q_DECLARE_METATYPE(mv::gui::StartPageConfigurationAction)

inline const auto startPageConfigurationActionMetaTypeId = qRegisterMetaType<mv::gui::StartPageConfigurationAction*>("mv::gui::StartPageConfigurationAction");
