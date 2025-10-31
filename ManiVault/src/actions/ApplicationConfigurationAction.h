// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GroupsAction.h"
#include "TriggerAction.h"
#include "BrandingConfigurationAction.h"
#include "StartPageConfigurationAction.h"
#include "ProjectsConfigurationAction.h"

namespace mv::gui {

/**
 * Application configuration action class
 *
 * Bundles application settings
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ApplicationConfigurationAction : public GroupsAction
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to /p parent object and /p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE ApplicationConfigurationAction(QObject* parent, const QString& title);

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

private:

    /**
     * Check whether the application configuration may be edited
     * @return True if the application configuration may be edited, false otherwise
     */
    bool mayConfigure();

signals:

    /**
     * Signals that the may configure state has changed to /p mayConfigure
     * @param mayConfigure New may configure state
     */
    void mayConfigureChanged(bool mayConfigure);

public: // Action getters

    TriggerAction& getConfigureAction() { return _configureAction; }
    BrandingConfigurationAction& getBrandingConfigurationAction() { return _brandingConfigurationAction; }
    StartPageConfigurationAction& getStartPageConfigurationAction() { return _startPageConfigurationAction; }
    ProjectsConfigurationAction& getProjectsConfigurationAction() { return _projectsConfigurationAction; }

private:
    TriggerAction                   _configureAction;                   /** Action for editing the application customization (visible when Ctrl + F8 is pressed) */
    BrandingConfigurationAction     _brandingConfigurationAction;       /** Branding configuration action */
    StartPageConfigurationAction    _startPageConfigurationAction;      /** Start page configuration action */
    ProjectsConfigurationAction     _projectsConfigurationAction;       /** Projects configuration action */
};

}

Q_DECLARE_METATYPE(mv::gui::ApplicationConfigurationAction)

inline const auto applicationConfigurationActionMetaTypeId = qRegisterMetaType<mv::gui::ApplicationConfigurationAction*>("mv::gui::ApplicationConfigurationAction");
