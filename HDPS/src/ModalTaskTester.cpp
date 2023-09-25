// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ModalTaskTester.h"
#include "TaskTesterRunner.h"
#include "ModalTask.h"

#include <QEventLoop>

namespace hdps
{

ModalTaskTester::ModalTaskTester(QObject* parent, const QString& name) :
    AbstractTaskTester(parent, name)
{
    testRunningIndeterminate();
    //testAggregation();
    testPerformance();
}

void ModalTaskTester::testRunningIndeterminate()
{
    TaskTesterRunner::createAndRun(this, [this](TaskTesterRunner* taskRunner) -> void {
        QEventLoop eventLoop(taskRunner);

        auto indeterminateTask = new ModalTask(taskRunner, "Indeterminate Task", nullptr, Task::Status::RunningIndeterminate);

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

        connect(indeterminateTask, &ModalTask::requestAbort, &timer, &QTimer::stop);

        connect(&timer, &QTimer::timeout, [&]() -> void {
            if (!subtasks.isEmpty()) {
                const auto subtaskName = subtasks.first();

                subtasks.removeFirst();

                indeterminateTask->setProgressDescription(subtaskName);
            }
            else {
                timer.stop();
                indeterminateTask->setFinished();
            }
        });

        timer.start();

        eventLoop.exec();
    });
}

void ModalTaskTester::testAggregation()
{
    TaskTesterRunner::createAndRun(this, [this](TaskTesterRunner* taskRunner) -> void {
        QEventLoop eventLoop(taskRunner);

        auto aggregateTask = new ModalTask(taskRunner, "Aggregate Task");

        QMap<QString, QTimer*> timers;

        const auto addChildTask = [this, taskRunner, &timers](const QString& name, QStringList tasks, int interval, Task* parentTask = nullptr) -> Task* {
            auto childTask = new ModalTask(nullptr, name);

            childTask->setParentTask(parentTask);

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

        auto childTaskA = addChildTask("A", {}, 0, aggregateTask);
        auto childTaskB = addChildTask("B", {}, 0, aggregateTask);

        eventLoop.exec();
    });
}

void ModalTaskTester::testPerformance()
{
    TaskTesterRunner::createAndRun(this, [this](TaskTesterRunner* taskRunner) -> void {
        QEventLoop eventLoop(taskRunner);

        auto performanceTask = new ModalTask(taskRunner, "Performance Task", nullptr, Task::Status::Running);

        const auto numberOfSubTasks = 10000;

        QStringList subtasks;

        subtasks.reserve(numberOfSubTasks);

        for (int subtaskIndex = 0; subtaskIndex < numberOfSubTasks; subtaskIndex++)
            subtasks << QString("Subtask: %1").arg(QString::number(subtaskIndex));

        performanceTask->setSubtasks(subtasks);

        QTimer timer;

        timer.setInterval(1);

        connect(performanceTask, &ModalTask::requestAbort, &timer, &QTimer::stop);

        connect(&timer, &QTimer::timeout, [&]() -> void {
            if (!subtasks.isEmpty()) {
                const auto subtaskName = subtasks.first();

                subtasks.removeFirst();

                performanceTask->setSubtaskFinished(subtaskName);
            }
            else {
                timer.stop();
                performanceTask->setFinished();
            }
            });

        timer.start();

        eventLoop.exec();
    });
}

}
