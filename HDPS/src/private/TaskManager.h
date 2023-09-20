// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractTaskManager.h"

namespace hdps
{

class TaskManager final : public AbstractTaskManager
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    TaskManager(QObject* parent = nullptr);

    /** Reset when destructed */
    ~TaskManager();

    /** Perform manager startup initialization */
    void initialize() override;

    /** Resets the contents of the data hierarchy manager */
    void reset() override;

    /**
     * Get all tasks
     * @return Vector of tasks
     */
    Tasks getTasks() override;

    /**
     * Adds \p task to the task manager
     * @param task Pointer to task to add
     */
    void addTask(Task* task) override;

    /**
     * Removes \p task from the task manager
     * @param task Pointer to task to remove
     */
    void removeTask(Task* task) override;

private:
    Tasks   _tasks;     /** Tasks registered */
};

}
