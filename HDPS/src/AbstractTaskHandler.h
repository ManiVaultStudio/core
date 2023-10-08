// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QObject>

namespace hdps::gui {
    class WidgetAction;
}

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

    /**
     * Get task
     * @return Pointer to task
     */
    virtual Task* getTask() final;

    /**
     * Get the minimum duration
     * @return Minimum duration before the handler displays a tasks UI
     */
    std::uint32_t getMinimumDuration() const;

    /**
     * Set the minimum duration
     * @param minimumDuration Minimum duration before the handler displays a tasks UI
     */
    void setMinimumDuration(std::uint32_t minimumDuration);

    /**
     * Get status bar action
     * @return Pointer to status bar widget action
     */
    virtual gui::WidgetAction* getStatusBarAction() = 0;

protected:

    /**
     * Set handle \p task
     * @param task Pointer to task to handle
     */
    virtual void setTask(Task* task) final;

signals:
    
    /**
     * Signals that the current task has changed from \p previousTask to \p currentTask
     * @param previousTask Pointer to previous task (if any)
     * @param currentTask Pointer to current task
     */
    void taskChanged(Task* previousTask, Task* currentTask);

    /**
     * Signals that the minimum duration changed from \p previousMinimumDuration to \p currentMinimumDuration
     * @param previousMinimumDuration Previous minimum duration
     * @param currentMinimumDuration Current minimum duration
     */
    void minimumDurationChanged(std::uint32_t previousMinimumDuration, std::uint32_t currentMinimumDuration);

private:
    Task*           _task;              /** Pointer to task to handle */
    std::uint32_t   _minimumDuration;   /** Minimum duration before the handler displays a tasks UI */
};

}
