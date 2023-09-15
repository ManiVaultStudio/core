// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ModalTaskTester.h"

#include <QEventLoop>

using namespace hdps;

ModalTaskTester::ModalTaskTester(QObject* parent, const QString& name) :
    AbstractTaskTester(parent, name),
    _modalTask(parent, name)
{
    connect(getTask(), &Task::statusChanged, this, [this](const Task::Status& previousStatus, const Task::Status& status) -> void {
        if (status == Task::Status::Aborting) {
            qDebug() << getTask()->getName() << "is aborting";

            getTimer().stop();

            deleteLater();
        }
    });
}

void ModalTaskTester::run()
{
}

hdps::Task* ModalTaskTester::getTask()
{
    return &_modalTask;
}

ModalTaskTesterSubtasks::ModalTaskTesterSubtasks(QObject* parent, const QString& name) :
    ModalTaskTester(parent, name),
    _tasks({ "Task A", "Task B", "Task C", "Task D", "Task E" })
{
    run();
}

void ModalTaskTesterSubtasks::run()
{
    getTask()->setSubtasks(_tasks);
    getTask()->setRunning();

    connect(&getTimer(), &QTimer::timeout, this, [&]() -> void {
        const auto subtaskName = _tasks.first();

        getTask()->setSubtaskFinished(subtaskName);

        _tasks.removeFirst();

        if (_tasks.isEmpty()) {
            getTimer().stop();
            getTask()->setFinished();
        }
        });

    getTimer().setInterval(1000);
    getTimer().start();
}

#include <Application.h>

ModalTaskTesterAggregate::ModalTaskTesterAggregate(QObject* parent, const QString& name) :
    ModalTaskTester(parent, name)
{
    TaskRunner::createAndRun(this, [this](TaskRunner* taskRunner) -> void {
        auto aggregateModalTask = new ModalTask(nullptr, "Aggregate Modal Task");
        auto modalTask = new ModalTask(aggregateModalTask, "Modal Task");

        QStringList subtasks{
            "Task A",
            "Task B",
            "Task C",
            "Task D",
            "Task E"
        };

        modalTask->setSubtasks(subtasks);
        modalTask->setRunning();

        for (const auto& subtask : subtasks) {
            taskRunner->thread()->sleep(2);
            
            modalTask->setSubtaskFinished(subtask);
        }

        taskRunner->thread()->sleep(2);

        modalTask->setFinished();
        //modalTask->deleteLater();
    });
}

void ModalTaskTesterAggregate::run()
{
}
