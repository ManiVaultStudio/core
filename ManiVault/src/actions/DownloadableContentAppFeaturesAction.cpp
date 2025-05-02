// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DownloadableContentAppFeaturesAction.h"

namespace mv::gui
{

DownloadableContentAppFeaturesAction::DownloadableContentAppFeaturesAction(QObject* parent) :
    VerticalGroupAction(parent, "Downloadable content", false),
    _projectsAppFeatureAction(this),
    _tutorialsAppFeatureAction(this),
    _videosAppFeatureAction(this)
{

    setShowLabels(false);

	setDefaultWidgetFlag(GroupAction::WidgetFlag::NoMargins);

    _projectsAppFeatureAction.getEnabledAction().setDefaultWidgetFlags(ToggleAction::ToggleImageText);
    _tutorialsAppFeatureAction.getEnabledAction().setDefaultWidgetFlags(ToggleAction::ToggleImageText);
    _videosAppFeatureAction.getEnabledAction().setDefaultWidgetFlags(ToggleAction::ToggleImageText);

    addAction(&_projectsAppFeatureAction);
    addAction(&_tutorialsAppFeatureAction);
    addAction(&_videosAppFeatureAction);
}

}
