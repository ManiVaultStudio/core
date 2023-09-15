// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <Task.h>

#include <QObject>
#include <QStringList>
#include <QTimer>
#include <QThread>
#include <QDebug>

class AbstractTaskTester : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE AbstractTaskTester(QObject* parent, const QString& name);

    virtual void run() {};
    virtual hdps::Task* getTask() {
        return nullptr;
    };

    static void registerTester(const QString taskTesterName);

    QTimer& getTimer();

    static QStringList getTesterNames();

private:
    QString     _name;
    QTimer      _timer;

protected:
    static QStringList testerNames;
};

class TaskRunnerPrivate : public  QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

signals:
    void finished();
    
    //void setSubtaskStarted(const QString& subtaskName, const QString& progressDescription = QString());
    //void setSubtaskFinished(std::uint32_t subtaskIndex, const QString& progressDescription = QString());
    //void setSubtaskFinished(const QString& subtaskName, const QString& progressDescription = QString());
    //void setFinished(bool toIdleWithDelay = true, std::uint32_t delay = TASK_DESCRIPTION_DISAPPEAR_INTERVAL);
};

class TaskRunner : public TaskRunnerPrivate
{
public:
    using RunFunction = std::function<void(TaskRunner*)>;

public:
    TaskRunner(QObject* parent, RunFunction runFunction) :
        TaskRunnerPrivate(parent),
        _runFunction(runFunction)
    {
        //connect(this, qOverload<std::uint32_t, const QString&>(&TaskRunnerPrivate::setSubtaskFinished), _task, qOverload<std::uint32_t, const QString&>(&TaskType::setSubtaskFinished));
        //connect(this, qOverload<const QString&, const QString&>(&TaskRunnerPrivate::setSubtaskFinished), _task, qOverload<const QString&, const QString&>(&TaskType::setSubtaskFinished));
        //connect(this, &TaskRunnerPrivate::setSubtaskStarted, _task, &TaskType::setSubtaskStarted);
        //connect(this, &TaskRunnerPrivate::setFinished, _task, &TaskType::setFinished);
    }

    void runTask() {
        _runFunction(this);

        emit finished();
    }

public:

    static void createAndRun(QObject* parent, RunFunction runFunction) {
        auto thread     = new QThread();
        auto taskRunner = new TaskRunner(nullptr, runFunction);

        taskRunner->moveToThread(thread);

        //connect(taskRunner, &Worker::error, this, &MyClass::errorString);
        connect(thread, &QThread::started, taskRunner, &TaskRunner::runTask);
        connect(taskRunner, &TaskRunner::finished, thread, &QThread::quit);
        connect(taskRunner, &TaskRunner::finished, taskRunner, &TaskRunner::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        thread->start();
    }

protected:
    RunFunction     _runFunction;
};