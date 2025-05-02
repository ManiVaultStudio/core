// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AppFeatureAction.h"

namespace mv::gui {

/**
 * Error logging app feature action class
 *
 * App feature action class for grouping error logging settings
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ErrorLoggingAppFeatureAction : public AppFeatureAction
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE ErrorLoggingAppFeatureAction(QObject* parent, const QString& title = "Error Logging");
};

}

Q_DECLARE_METATYPE(mv::gui::ErrorLoggingAppFeatureAction)

inline const auto errorLoggingAppFeatureActionMetaTypeId = qRegisterMetaType<mv::gui::ErrorLoggingAppFeatureAction*>("mv::gui::ErrorLoggingAppFeatureAction");
