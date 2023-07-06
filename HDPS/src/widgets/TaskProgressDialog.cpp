// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TaskProgressDialog.h"

#include <QDebug>
#include <QCoreApplication>

namespace hdps::gui {

TaskProgressDialog::TaskProgressDialog(QWidget* parent, const QStringList& tasks, const QString& title, const QIcon& icon) :
    QProgressDialog(parent),
    _tasks(tasks)
{
    setWindowIcon(icon);
    setWindowTitle(title);
    setWindowModality(Qt::WindowModal);
    setMinimumDuration(0);
    setFixedWidth(600);
    setMinimum(0);
    setMaximum(_tasks.count());
    setValue(0);
    setAutoClose(false);
    setAutoReset(false);
}

void TaskProgressDialog::addTasks(const QStringList& tasks)
{
    _tasks << tasks;

    setMaximum(_tasks.count());
}

void TaskProgressDialog::setCurrentTask(const QString& taskName)
{
    setLabelText(taskName);

    QCoreApplication::processEvents();
}

void TaskProgressDialog::setTaskFinished(const QString& taskName)
{
    _tasks.removeOne(taskName);

    setValue(maximum() - _tasks.count());

    QCoreApplication::processEvents();
}

void TaskProgressDialog::setCurrentTaskText(const QString& taskText)
{
    setLabelText(taskText);

    QCoreApplication::processEvents();
}

}
