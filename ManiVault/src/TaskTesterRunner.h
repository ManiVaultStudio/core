// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QObject>
#include <QThread>
#include <QDebug>

namespace mv
{

class TaskTesterRunnerPrivate : public  QObject
{
    Q_OBJECT

protected:
    using QObject::QObject;

signals:
    void finished();

    friend class TaskTesterRunner;
};

class TaskTesterRunner : public TaskTesterRunnerPrivate
{
public:
    using RunFunction = std::function<void(TaskTesterRunner*)>;

public:
    TaskTesterRunner(QObject* parent, RunFunction runFunction);

    void runTask();

public:

    static void createAndRun(QObject* parent, RunFunction runFunction);

protected:
    RunFunction     _runFunction;
};

}
