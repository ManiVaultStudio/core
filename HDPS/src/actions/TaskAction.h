// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GroupAction.h"
#include "ProgressAction.h"
#include "TriggerAction.h"
#include "Task.h"

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
    Q_INVOKABLE TaskAction(QObject* parent, const QString& title);

    /**
     * get task
     * @return Pointer to task to keep track of
     */
    Task* getTask();

    /**
     * Set task to \p task
     * @param task Pointer to task to keep track of
     */
    void setTask(Task* task);

    /**
     * Get progress action
     * @return Reference to built-in progress action
     */
    ProgressAction& getProgressAction();

    /**
     * Get kill task action
     * @return Reference to built-in trigger action for killing the task
     */
    gui::TriggerAction& getKillTaskAction();

private:

    /** Updates the read-only status of the progress and kill task actions */
    void updateActionsReadOnly();

    /** Updates the range of the progress action */
    void updateProgressActionRange();

    /** Updates the progress action text format, depending on the task status */
    void updateProgressActionTextFormat();

    /** Updates the visibility of the task killer action */
    void updateKillTaskActionVisibility();

signals:

    /**
     * Signals that the current task changed to \p task
     * @param previousTask Pointer to previous task
     * @param task Pointer to current task
     */
    void taskChanged(Task* previousTask, Task* task);

private:
    ProgressAction      _progressAction;    /** Progress action */
    gui::TriggerAction  _killTaskAction;    /** Kill task action */
    Task*               _task;              /** Pointer to task to keep track of */

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(hdps::gui::TaskAction)

inline const auto taskActionMetaTypeId = qRegisterMetaType<hdps::gui::TaskAction*>("hdps::gui::TaskAction");
