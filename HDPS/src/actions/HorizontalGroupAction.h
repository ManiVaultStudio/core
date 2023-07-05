// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GroupAction.h"

namespace hdps::gui {

/**
 * Horizontal group action class
 *
 * Convenience group action for displaying child actions horizontally
 *
 * @author Thomas Kroes
 */
class HorizontalGroupAction : public GroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param expanded Whether the group is initially expanded/collapsed
     */
    Q_INVOKABLE HorizontalGroupAction(QObject* parent, const QString& title, const bool& expanded = false);
};

}

Q_DECLARE_METATYPE(hdps::gui::HorizontalGroupAction)

inline const auto horizontalGroupActionMetaTypeId = qRegisterMetaType<hdps::gui::HorizontalGroupAction*>("hdps::gui::HorizontalGroupAction");