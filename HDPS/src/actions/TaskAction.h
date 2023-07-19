// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GroupAction.h"
#include "ProgressAction.h"
#include "TriggerAction.h"

namespace hdps::gui {

/**
 * Task action class
 *
 * Task action class for interacting with a task
 *
 * @author Thomas Kroes
 */
class TaskAction : public GroupAction
{
    Q_OBJECT

public:

    /** Describes the widget flags */
    enum WidgetFlag {

        ProgressBar = 0x00100,  /** Widget includes a progress bar */
        KillButton  = 0x00200,  /** Widget includes a button to kill the task */

        Default = ProgressBar | KillButton
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE TaskAction(QObject* parent, const QString& title = "");

private:
    ProgressAction      _progressAction;    /** Progress action */
    gui::TriggerAction  _killTaskAction;    /** Kill task action */

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(hdps::gui::TaskAction)

inline const auto taskActionMetaTypeId = qRegisterMetaType<hdps::gui::TaskAction*>("hdps::gui::TaskAction");
