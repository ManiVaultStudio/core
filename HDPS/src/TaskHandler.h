// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QObject>

namespace hdps {

class Task;

/**
 * Task handler class
 *
 * Class for interacting with a task.
 *
 * @author Thomas Kroes
 */
class TaskHandler : public QObject
{
    Q_OBJECT

public:

    /**
    * Construct task handler with \p parent object
    * @param parent Pointer to parent object
    */
    TaskHandler(QObject* parent);

    /**
     * Get task
     * @return Pointer to task
     */
    Task* getTask();

protected:

    /**
     * Set handle \p task
     * @param task Pointer to task to handle
     */
    virtual void setTask(Task* task) final;

private:
    Task*   _task;    /** Pointer to task to handle */
};

}
