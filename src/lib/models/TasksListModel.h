// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractTasksModel.h"

namespace mv
{

/**
 * Tasks model class
 *
 * Standard item model class for tasks
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT TasksListModel : public AbstractTasksModel
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    TasksListModel(QObject* parent = nullptr);

    /**
     * Get task by \p rowIndex
     * @param rowIndex Row index of the task to get
     * @return Pointer to task at \p rowIndex, or nullptr if no task is found
     */
    QPointer<Task> getTask(const std::int32_t& rowIndex) const;

private:

    /**
     * Add \p task to the model (this method is called when a task is added to the manager)
     * @param task Pointer to task to add
     */
    void addTask(Task* task) override;
};

}
