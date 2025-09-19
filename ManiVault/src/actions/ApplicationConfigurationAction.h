// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "VerticalGroupAction.h"

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

public: // Action getters

    TriggerAction& getCustomizeAction() { return _configureAction; }
    StringAction& getNameAction() { return _nameAction; }

private:
    gui::TriggerAction  _configureAction;   /** Action for editing the application customization (visible when Ctrl + F8 is pressed) */
    StringAction        _nameAction;        /** Application name */
};

}

Q_DECLARE_METATYPE(mv::gui::ApplicationConfigurationAction)

inline const auto applicationConfigurationActionMetaTypeId = qRegisterMetaType<mv::gui::ApplicationConfigurationAction*>("mv::gui::ApplicationConfigurationAction");
