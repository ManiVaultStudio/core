// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "StatusBarAction.h"

#include <actions/HorizontalGroupAction.h>
#include <actions/StringAction.h>

/**
 * Start page status bar action class
 *
 * Status bar action class for switching to the start page
 *
 * @author Thomas Kroes
 */
class StartPageStatusBarAction : public StatusBarAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    StartPageStatusBarAction(QObject* parent, const QString& title);

private:
    mv::gui::HorizontalGroupAction  _barGroupAction;    /** Bar group action */
    mv::gui::StringAction           _iconAction;        /** String action for showing the start page icon with FontAwesome */
};
