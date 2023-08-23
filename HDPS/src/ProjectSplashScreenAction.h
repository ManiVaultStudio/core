// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/AbstractSplashScreenAction.h"
#include "actions/VerticalGroupAction.h"
#include "actions/ImageAction.h"

#include "widgets/SplashScreenDialog.h"

class QMainWindow;

namespace hdps {
    class ProjectMetaAction;
}

namespace hdps::gui {

/**
 * Project splash screen action class
 *
 * Action class for configuring and displaying a project splash screen.
 *
 * @author Thomas Kroes
 */
class ProjectSplashScreenAction final : public AbstractSplashScreenAction
{
public:

    /**
     * Construct with owning \p projectMetaAction
     * @param projectMetaAction Pointer to owning project meta action
     */
    ProjectSplashScreenAction(ProjectMetaAction* projectMetaAction);

    /** Get an instance of a splash screen dialog */
    SplashScreenDialog* getSplashScreenDialog() override;

public: // Serialization

    /**
     * Load project from variant
     * @param Variant representation of the project
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save project to variant
     * @return Variant representation of the project
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    const GroupAction& getEditAction() const { return _editAction; }
    const ImageAction& getProjectImageAction() const { return _projectImageAction; }
    const ImageAction& getAffiliateLogosImageAction() const { return _affiliateLogosImageAction; }

    GroupAction& getEditAction() { return _editAction; }
    ImageAction& getProjectImageAction() { return _projectImageAction; }
    ImageAction& getAffiliateLogosImageAction() { return _affiliateLogosImageAction; }

private:
    ProjectMetaAction*      _projectMetaAction;             /** Pointer to owning project meta action */
    VerticalGroupAction     _editAction;                    /** Vertical group action for editing the splash screen */
    ImageAction             _projectImageAction;            /** Image action for the project image */
    ImageAction             _affiliateLogosImageAction;     /** Image action for the affiliate logo's image */

    friend class hdps::ProjectMetaAction;
};

}
