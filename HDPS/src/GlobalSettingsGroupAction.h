// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/GroupAction.h"

namespace hdps
{

/**
 * Global settings group action class
 *
 * Base action class which groups related settings
 *
 * @author Thomas Kroes
 */
class GlobalSettingsGroupAction : public hdps::gui::GroupAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param expanded Boolean determining whether the group action is expanded by default
     */
    GlobalSettingsGroupAction(QObject* parent, const QString& title, bool expanded = true);

    /** Get settings prefix for child actions */
    QString getSettingsPrefix() const;
};

}
