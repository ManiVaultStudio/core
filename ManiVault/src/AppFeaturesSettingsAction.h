// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GlobalSettingsGroupAction.h"

#include "actions/ErrorLoggingAppFeatureAction.h"
#include "actions/DownloadableContentAppFeaturesAction.h"

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
    const DownloadableContentAppFeaturesAction& getDownloadableContentAppFeaturesAction() const { return _downloadableContentAppFeaturesAction; }

private:
    ErrorLoggingAppFeatureAction            _errorLoggingAppFeatureAction;          /** App feature action for configuring error logging */
    DownloadableContentAppFeaturesAction    _downloadableContentAppFeaturesAction;  /** App feature action for configuring downloadable dynamic content */
};

}
