// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TaskTesterRunner.h"

namespace mv
{

TaskTesterRunner::TaskTesterRunner(QObject* parent, RunFunction runFunction) :
    TaskTesterRunnerPrivate(parent),
    _runFunction(runFunction)
{
}

void TaskTesterRunner::runTask()
{
    _runFunction(this);

    emit finished();
}

void TaskTesterRunner::createAndRun(QObject* parent, RunFunction runFunction)
{
    auto thread     = new QThread();
    auto taskRunner = new TaskTesterRunner(nullptr, runFunction);

    taskRunner->moveToThread(thread);

    //connect(taskRunner, &Worker::error, this, &MyClass::errorString);
    connect(thread, &QThread::started, taskRunner, &TaskTesterRunner::runTask);
    connect(taskRunner, &TaskTesterRunner::finished, thread, &QThread::quit);
    connect(taskRunner, &TaskTesterRunner::finished, taskRunner, &TaskTesterRunner::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();
}

}
