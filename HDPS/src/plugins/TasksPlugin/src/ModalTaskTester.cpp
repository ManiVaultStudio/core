// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ModalTaskTester.h"

#include <QEventLoop>

using namespace hdps;

ModalTaskTester::ModalTaskTester(QObject* parent, const QString& name) :
    AbstractTaskTester(parent, name)
{
    testRunningIndeterminate();
    testAggregation();
}

void ModalTaskTester::testRunningIndeterminate()
{
    TaskRunner::createAndRun(this, [this](TaskRunner* taskRunner) -> void {
        QEventLoop eventLoop(taskRunner);

        auto indeterminateModalTask = new ModalTask(taskRunner, "Indeterminate", Task::Status::RunningIndeterminate);

        auto subtasks = QStringList({
            "Step 1",
            "Step 2",
            "Step 3",
            "Step 4",
            "Step 5",
            "Step 6",
            "Step 7",
            "Step 8",
            "Step 9",
            "Step 10"
        });

        QTimer timer;

        timer.setInterval(1000);

        connect(indeterminateModalTask, &ModalTask::requestAbort, &timer, &QTimer::stop);

        connect(&timer, &QTimer::timeout, [&]() -> void {
            if (!subtasks.isEmpty()) {
                const auto subtaskName = subtasks.first();

                subtasks.removeFirst();

                indeterminateModalTask->setProgressDescription(subtaskName);
            }
            else {
                timer.stop();
                indeterminateModalTask->setFinished();
            }
        });

        timer.start();

        eventLoop.exec();
    });
}

void ModalTaskTester::testAggregation()
{
    TaskRunner::createAndRun(this, [this](TaskRunner* taskRunner) -> void {
        QEventLoop eventLoop(taskRunner);

        auto aggregateModalTask = new ModalTask(taskRunner, "Aggregate Modal Task");

        QMap<QString, QTimer*> timers;

        const auto addChildTask = [this, taskRunner, &timers](const QString& name, QStringList tasks, int interval, Task* parentTask = nullptr) -> Task* {
            auto childTask = new ModalTask(parentTask, name);

            if (!tasks.isEmpty()) {
                childTask->setSubtasks(tasks);
                childTask->setRunning();

                auto timer = new QTimer();

                timers[name] = timer;

                timer->setInterval(interval);

                connect(childTask, &ModalTask::requestAbort, timer, &QTimer::stop);

                connect(timer, &QTimer::timeout, [timer, childTask, &tasks]() -> void {
                    if (!tasks.isEmpty()) {
                        const auto subtaskName = tasks.first();

                        tasks.removeFirst();

                        childTask->setSubtaskFinished(subtaskName);
                    }
                    else {
                        timer->stop();
                        childTask->setFinished();
                    }
                    });

                timer->start();
            }
            else {
                childTask->setRunning();
            }

            return childTask;
        };

        auto childTaskA = addChildTask("A", {}, 0, aggregateModalTask);
        auto childTaskB = addChildTask("B", {}, 0, aggregateModalTask);

        auto childTaskAA = addChildTask("AA", {
            "Task 1",
            "Task 2",
            "Task 3",
            "Task 4",
            "Task 5",
            "Task 6",
            "Task 7",
            "Task 8",
            "Task 9",
            "Task 10"
            }, 700, childTaskA);

        auto childTaskAB = addChildTask("AB", {
            "Task 1",
            "Task 2",
            "Task 3",
            "Task 4",
            "Task 5",
            "Task 6",
            "Task 7",
            "Task 8",
            "Task 9",
            "Task 10"
            }, 850, childTaskA);

        auto childTaskBA = addChildTask("BA", {
            "Task 1",
            "Task 2",
            "Task 3",
            "Task 4",
            "Task 5",
            "Task 6",
            "Task 7",
            "Task 8",
            "Task 9",
            "Task 10"
            }, 950, childTaskB);

        auto childTaskBB = addChildTask("BB", {
            "Task 1",
            "Task 2",
            "Task 3",
            "Task 4",
            "Task 5",
            "Task 6",
            "Task 7",
            "Task 8",
            "Task 9",
            "Task 10"
            }, 800, childTaskB);

        eventLoop.exec();
    });
}
