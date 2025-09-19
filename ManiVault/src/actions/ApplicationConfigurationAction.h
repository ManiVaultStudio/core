// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "VerticalGroupAction.h"
#include "ImageAction.h"
#include "StringAction.h"
#include "ToggleAction.h"
#include "StartPageConfigurationAction.h"

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
class CORE_EXPORT ApplicationConfigurationAction : public VerticalGroupAction
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

public: // Action getters

    TriggerAction& getCustomizeAction() { return _configureAction; }
    StringAction& getBaseNameAction() { return _baseNameAction; }
    StringAction& getFullNameAction() { return _fullNameAction; }
    ToggleAction& getEditFullNameAction() { return _editFullNameAction; }
    ImageAction& getLogoAction() { return _logoAction; }
    StartPageConfigurationAction& getStartPageConfigurationAction() { return _startPageConfigurationAction; }

private:
    TriggerAction                   _configureAction;               /** Action for editing the application customization (visible when Ctrl + F8 is pressed) */
    StringAction                    _baseNameAction;                /** Application base name action (e.g. main window title) */
    StringAction                    _fullNameAction;                /** Application full name action (e.g. start page title) */
    ToggleAction                    _editFullNameAction;            /** Whether to edit the full name (otherwise it is derived from base name and version) */
    ImageAction                     _logoAction;                    /** Application logo action */
    StartPageConfigurationAction    _startPageConfigurationAction;  /** Start page configuration action */
};

}

Q_DECLARE_METATYPE(mv::gui::ApplicationConfigurationAction)

inline const auto applicationConfigurationActionMetaTypeId = qRegisterMetaType<mv::gui::ApplicationConfigurationAction*>("mv::gui::ApplicationConfigurationAction");
