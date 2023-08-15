// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QObject>

namespace hdps {

class Task;

/**
 * Abstract task handler class
 *
 * Base handler class for interacting with a task.
 *
 * @author Thomas Kroes
 */
class AbstractTaskHandler : public QObject
{
    Q_OBJECT

public:

    /**
    * Construct task handler with \p parent object
    * @param parent Pointer to parent object
    * @param task Pointer to task to handle
    */
    AbstractTaskHandler(QObject* parent, Task* task);

    /** Initializes the handler */
    virtual void init() = 0;

    /**
     * Get task
     * @return Pointer to task
     */
    virtual Task* getTask() final;

protected:

    /**
     * Set handle \p task
     * @param task Pointer to task to handle
     */
    virtual void setTask(Task* task) final;

signals:
    
    /**
     * Signals that the current task has changed to \p currentTask
     * @param previousTask Pointer to previous task (if any)
     * @param currentTask Pointer to current task
     */
    void taskChanged(Task* previousTask, Task* currentTask);

private:
    Task*   _task;    /** Pointer to task to handle */
};

}
