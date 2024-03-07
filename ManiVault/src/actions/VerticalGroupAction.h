// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GroupAction.h"

namespace mv::gui {

/**
 * Vertical group action class
 *
 * Convenience group action for displaying child actions vertically
 *
 * @author Thomas Kroes
 */
class VerticalGroupAction : public GroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param expanded Whether the group is initially expanded/collapsed
     */
    Q_INVOKABLE VerticalGroupAction(QObject* parent, const QString& title, const bool& expanded = false);
};

}

Q_DECLARE_METATYPE(mv::gui::VerticalGroupAction)

inline const auto verticalGroupActionMetaTypeId = qRegisterMetaType<mv::gui::VerticalGroupAction*>("mv::gui::VerticalGroupAction");