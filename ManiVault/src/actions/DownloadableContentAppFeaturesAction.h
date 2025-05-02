// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/VerticalGroupAction.h"

#include "actions/ProjectsAppFeatureAction.h"
#include "actions/TutorialsAppFeatureAction.h"
#include "actions/VideosAppFeatureAction.h"

namespace mv::gui
{

/**
 * Downloadable content app features action class
 *
 * Groups all app features for downloadable content
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT DownloadableContentAppFeaturesAction final : public VerticalGroupAction
{
public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    DownloadableContentAppFeaturesAction(QObject* parent);

public: // Action getters

    ProjectsAppFeatureAction& getProjectsAppFeatureAction() { return _projectsAppFeatureAction; }
    TutorialsAppFeatureAction& getTutorialsAppFeatureAction() { return _tutorialsAppFeatureAction; }
    VideosAppFeatureAction& getVideosAppFeatureAction() { return _videosAppFeatureAction; }

private:
    ProjectsAppFeatureAction    _projectsAppFeatureAction;      /** App feature action for configuring projects dynamic content */
    TutorialsAppFeatureAction   _tutorialsAppFeatureAction;     /** App feature action for configuring tutorials dynamic content */
    VideosAppFeatureAction      _videosAppFeatureAction;        /** App feature action for configuring videos dynamic content */
};

}
