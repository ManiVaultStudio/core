// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QProgressDialog>

namespace hdps::gui {

/**
 * Task progress dialog class
 *
 * Class for reporting task progress.
 *
 * @author Thomas Kroes
 */
class TaskProgressDialog : public QProgressDialog
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     * @param tasks List of task names
     * @param title Main task title
     * @param icon Dialog icon
     */
    explicit TaskProgressDialog(QWidget* parent, const QStringList& tasks, const QString& title, const QIcon& icon);

    /**
     * Add a list of tasks
     * @param tasks List of tasks to add
     */
    void addTasks(const QStringList& tasks);

    /**
     * Set the name of the current task
     * @param taskName Name of the current task
     */
    void setCurrentTask(const QString& taskName);

    /**
     * Flag task as finished
     * @param taskName Name of the task that finished
     */
    void setTaskFinished(const QString& taskName);

    /**
     * Set the current tasks text (visible in the label on the progress dialog)
     * @param taskText Task text
     */
    void setCurrentTaskText(const QString& taskText);

protected:
    QStringList     _tasks;     /** String list of tasks that need to be performed */
};

}
