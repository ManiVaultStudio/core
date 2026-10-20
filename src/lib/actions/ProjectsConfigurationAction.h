// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "VerticalGroupAction.h"
#include "StringsAction.h"

namespace mv::gui {

/**
 * Application projects configuration action class
 *
 * Bundles application projects settings
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ProjectsConfigurationAction : public VerticalGroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to /p parent object and /p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE ProjectsConfigurationAction(QObject* parent, const QString& title);

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

    StringsAction& getProjectsDsnsAction() { return _projectsDsnsAction; }

private:
    StringsAction       _projectsDsnsAction;    /** Projects DSNs action */
};

}

Q_DECLARE_METATYPE(mv::gui::ProjectsConfigurationAction)

inline const auto projectsConfigurationActionMetaTypeId = qRegisterMetaType<mv::gui::ProjectsConfigurationAction*>("mv::gui::ProjectsConfigurationAction");
