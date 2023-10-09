// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractTasksModel.h"

namespace hdps
{

/**
 * Tasks model class
 *
 * Standard item model class for tasks
 *
 * @author Thomas Kroes
 */
class TasksListModel : public AbstractTasksModel
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    TasksListModel(QObject* parent = nullptr);

private:

    /**
     * Add \p task to the model (this method is called when a task is added to the manager)
     * @param task Pointer to task to add
     */
    void addTask(Task* task) override;
};

}
