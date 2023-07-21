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
    */
    AbstractTaskHandler(QObject* parent);

    /** Initializes the handler */
    virtual void init() = 0;

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
