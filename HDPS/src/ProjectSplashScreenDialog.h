// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "widgets/SplashScreenDialog.h"

namespace hdps {
    class ProjectMetaAction;
}

namespace hdps::gui {

/**
 * Project splash screen dialog class
 *
 * Dialog class for displaying project information in a splash screen
 *
 * @author Thomas Kroes
 */
class ProjectSplashScreenDialog final : public SplashScreenDialog
{
public:

    /**
     * Construct with \p projectMetaAction
     * @param projectMetaAction Pointer to project meta action
     */
    ProjectSplashScreenDialog(ProjectMetaAction* projectMetaAction, QWidget* parent = nullptr);
    
private:

    void createTopContent();
    void createCenterContent();
    void createBottomContent();

private:
    ProjectMetaAction*  _projectMetaAction;     /** Project meta action which contains the project information to display on the splash screen dialog */
    QPixmap             _backgroundImage;       /** Background image */
    QVBoxLayout         _mainLayout;            /** Main vertical layout */
    QToolButton         _closeToolButton;       /** Button for forcefully closing the splash screen */

    friend class ProjectSplashScreenAction;
};

}
