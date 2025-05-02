// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "VerticalGroupAction.h"

namespace mv::gui {

/**
 * App feature action class
 *
 * Base vertical group action class for grouping app feature settings
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT AppFeatureAction : public WidgetAction
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE AppFeatureAction(QObject* parent, const QString& title);

};

}

Q_DECLARE_METATYPE(mv::gui::AppFeatureAction)

inline const auto appFeatureActionMetaTypeId = qRegisterMetaType<mv::gui::AppFeatureAction*>("mv::gui::AppFeatureAction");
