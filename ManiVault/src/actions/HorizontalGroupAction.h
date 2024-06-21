// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GroupAction.h"

namespace mv::gui {

/**
 * Horizontal group action class
 *
 * Convenience group action for displaying child actions horizontally
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT HorizontalGroupAction : public GroupAction
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

public: // Miscellaneous

    /**
     * Create example widget
     * @param parent Parent widget
     * @return Pointer to widget (maybe a nullptr)
     */
    QWidget* createExampleWidget(QWidget* parent) const override;
};

}

Q_DECLARE_METATYPE(mv::gui::HorizontalGroupAction)

inline const auto horizontalGroupActionMetaTypeId = qRegisterMetaType<mv::gui::HorizontalGroupAction*>("mv::gui::HorizontalGroupAction");