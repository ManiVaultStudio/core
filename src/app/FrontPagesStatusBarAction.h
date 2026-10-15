// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/StatusBarAction.h>

/**
 * Front pages status bar action class
 *
 * Status bar action class for heading to the start page and help front page
 *
 * @author Thomas Kroes
 */
class FrontPagesStatusBarAction : public mv::gui::StatusBarAction
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    FrontPagesStatusBarAction(QObject* parent, const QString& title);
};
