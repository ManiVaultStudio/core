// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ModalTaskTestRunningIndeterminate.h"

using namespace hdps;

ModalTaskTestRunningIndeterminate::ModalTaskTestRunningIndeterminate(QObject* parent, const QString& name) :
    ModalTaskTester(parent, name),
    _tasks({ "Task A", "Task B", "Task C", "Task D", "Task E" })
{
    connect(getTask(), &Task::statusChanged, this, [this](const Task::Status& previousStatus, const Task::Status& status) -> void {
        if (status == Task::Status::Aborting) {
            qDebug() << getTask()->getName() << "is aborting";

            getTimer().stop();

            deleteLater();
        }
    });

    run();
}

void ModalTaskTestRunningIndeterminate::run()
{
    getTask()->setRunningIndeterminate();

    connect(&getTimer(), &QTimer::timeout, this, [&]() -> void {
        getTask()->setProgressDescription(_tasks.first());

        _tasks.removeFirst();

        if (_tasks.isEmpty()) {
            getTimer().stop();
            getTask()->setFinished();
        }
        });

    getTimer().setInterval(1000);
    getTimer().start();
}
