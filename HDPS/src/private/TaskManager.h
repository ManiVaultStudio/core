// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractTaskManager.h"

namespace hdps
{

class TaskManager final : public AbstractTaskManager
{
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
     * Adds \p task to the task manager
     * @param task Pointer to task to add
     */
    void addTask(Task* task) override;

    /**
     * Removes \p task from the task manager
     * @param task Pointer to task to remove
     */
    void removeTask(Task* task) override;

    /**
     * Get tasks group action
     * @return Reference to group action
     */
    gui::GroupAction& getTasksGroupAction() override;

private:
    gui::GroupAction    _tasksGroupAction;      /** Groups all task actions */
};

}