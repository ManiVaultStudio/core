// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "widgets/SplashScreenDialog.h"

namespace hdps::gui {

class ApplicationSplashScreenAction;

/**
 * Application splash screen dialog class
 *
 * Dialog class for displaying application information.
 *
 * @author Thomas Kroes
 */
class ApplicationSplashScreenDialog final : public SplashScreenDialog
{
public:

    /**
     * Construct with \p applicationSplashScreenAction
     * @param applicationSplashScreenAction Pointer to application splash screen action
     * @param parent Pointer to parent object
     */
    ApplicationSplashScreenDialog(ApplicationSplashScreenAction* applicationSplashScreenAction, QWidget* parent = nullptr);

private:

    void createCenterContent();
    void createBottomContent();

private:
    ApplicationSplashScreenAction*  _applicationSplashScreenAction;     /** Pointer to owning application splash screen action */
    QVBoxLayout                     _mainLayout;                        /** Main vertical layout */
    QToolButton                     _closeToolButton;                   /** Button for forcefully closing the splash screen */

    friend class ProjectSplashScreenAction;
};

}
