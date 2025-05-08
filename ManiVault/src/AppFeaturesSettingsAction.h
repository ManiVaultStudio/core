// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GlobalSettingsGroupAction.h"

#include "actions/ErrorLoggingAppFeatureAction.h"
#include "actions/ProjectsAppFeatureAction.h"
#include "actions/TutorialsAppFeatureAction.h"
#include "actions/VideosAppFeatureAction.h"

namespace mv::gui
{

/**
 * App features settings action class
 *
 * Groups all app features settings for the application
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT AppFeaturesSettingsAction final : public GlobalSettingsGroupAction
{
public:

    /**
     * Construct the error logging settings action with a parent object
     * @param parent Pointer to parent object
     */
    AppFeaturesSettingsAction(QObject* parent);

public: // Action getters

    const ErrorLoggingAppFeatureAction& getErrorLoggingAppFeatureAction() const { return _errorLoggingAppFeatureAction; }
    const ProjectsAppFeatureAction& getProjectsAppFeatureAction() const { return _projectsAppFeatureAction; }
    const TutorialsAppFeatureAction& getTutorialsAppFeatureAction() const { return _tutorialsAppFeatureAction; }
    const VideosAppFeatureAction& getVideosAppFeatureAction() const { return _videosAppFeatureAction; }

private:
    ErrorLoggingAppFeatureAction    _errorLoggingAppFeatureAction;      /** App feature action for configuring error logging */
    ProjectsAppFeatureAction        _projectsAppFeatureAction;          /** App feature action for configuring projects dynamic content */
    TutorialsAppFeatureAction       _tutorialsAppFeatureAction;         /** App feature action for configuring tutorials dynamic content */
    VideosAppFeatureAction          _videosAppFeatureAction;            /** App feature action for configuring videos dynamic content */
};

}
