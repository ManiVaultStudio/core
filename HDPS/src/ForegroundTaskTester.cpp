// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ForegroundTaskTester.h"
#include "TaskTesterRunner.h"
#include "ForegroundTask.h"
#include "CoreInterface.h"

#include <QEventLoop>
#include <QSharedPointer>

namespace mv
{

ForegroundTaskTester::ForegroundTaskTester(QObject* parent, const QString& name) :
    AbstractTaskTester(parent, name)
{
    testRunningIndeterminate();
    testPerformance();
}

void ForegroundTaskTester::testRunningIndeterminate()
{
    TaskTesterRunner::createAndRun(this, [this](TaskTesterRunner* taskRunner) -> void {
        QEventLoop eventLoop(taskRunner);

        auto indeterminateTask = std::make_unique<ForegroundTask>(taskRunner, "Indeterminate Task", Task::Status::RunningIndeterminate);

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

        connect(indeterminateTask.get(), &ModalTask::requestAbort, this, [this, &timer, &indeterminateTask]() -> void {
            timer.stop();
            indeterminateTask->setAborted();
        });

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

void ForegroundTaskTester::testPerformance()
{
    TaskTesterRunner::createAndRun(this, [this](TaskTesterRunner* taskRunner) -> void {
        QEventLoop eventLoop(taskRunner);

        auto performanceTask = std::make_unique<ForegroundTask>(taskRunner, "Performance Task", Task::Status::Running);

        performanceTask->setMayKill(true);

        const auto numberOfSubTasks = 10000;

        QStringList subtasks;

        subtasks.reserve(numberOfSubTasks);

        for (int subtaskIndex = 0; subtaskIndex < numberOfSubTasks; subtaskIndex++)
            subtasks << QString("Subtask: %1").arg(QString::number(subtaskIndex));

        performanceTask->setSubtasks(subtasks);

        QTimer timer;

        timer.setInterval(1);

        connect(performanceTask.get(), &ModalTask::requestAbort, this, [this, &timer, &performanceTask]() -> void {
            timer.stop();
            performanceTask->setAborted();
        });

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
