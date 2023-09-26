// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "BackgroundTaskTester.h"
#include "TaskTesterRunner.h"
#include "BackgroundTask.h"
#include "CoreInterface.h"

#include <QEventLoop>

namespace hdps
{

BackgroundTaskTester::BackgroundTaskTester(QObject* parent, const QString& name) :
    AbstractTaskTester(parent, name)
{
    TaskTesterRunner::createAndRun(this, [this](TaskTesterRunner* taskRunner) -> void {
        QEventLoop eventLoop(taskRunner);

        QMap<QString, QTimer*> timers;

        const auto addChildTask = [this, &timers](const QString& name, QStringList tasks, int interval) -> Task* {
            auto childTask = new BackgroundTask(nullptr, name);

            //childTask->setParentTask(Application::current()->getTask(Application::TaskType::OverallBackground));

            if (!tasks.isEmpty()) {
                childTask->setSubtasks(tasks);
                childTask->setRunning();

                auto timer = new QTimer();

                timers[name] = timer;

                timer->setInterval(interval);

                connect(childTask, &BackgroundTask::requestAbort, timer, &QTimer::stop);

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

        auto childTaskA = addChildTask("Reading large file", {
            "Import file 1",
            "Import file 2",
            "Import file 3",
            "Import file 4",
            "Import file 5",
            "Import file 6",
            "Import file 7",
            "Import file 8",
            "Import file 9",
            "Import file 10"
            }, 502);

        auto childTaskB = addChildTask("Updating ManiVault", {
            "Downloading update 1",
            "Downloading update 2",
            "Downloading update 3",
            "Downloading update 4",
            "Downloading update 5",
            "Downloading update 6",
            "Downloading update 7",
            "Downloading update 8",
            "Downloading update 9",
            "Downloading update 10"
            }, 850);

        eventLoop.exec();

        delete childTaskA;
        delete childTaskB;
    });
}

}
