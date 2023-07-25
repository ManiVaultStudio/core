// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Task.h"
#include "CoreInterface.h"

namespace hdps {

using namespace util;

QMap<Task::Status, QString> Task::statusNames = QMap<Status, QString>({
    { Task::Status::Undefined, "Undefined" },
    { Task::Status::Idle, "Idle" },
    { Task::Status::Running, "Running" },
    { Task::Status::Finished, "Finished" },
    { Task::Status::Aborted, "Aborted" }
});

Task::Task(QObject* parent, const QString& name, const Status& status /*= Status::Idle*/, AbstractTaskHandler* handler /*= nullptr*/) :
    QObject(parent),
    Serializable(name),
    _name(name),
    _description(),
    _status(status),
    _handler(handler),
    _progress(0.f),
    _subtasks(),
    _subtasksDescriptions()
{
    tasks().addTask(this);
}

Task::~Task()
{
    tasks().removeTask(this);
}

QString Task::getName() const
{
    return _name;
}

void Task::setName(const QString& name)
{
    if (name == _name)
        return;

    _name = name;

    emit nameChanged(_name);
}

QString Task::getDescription() const
{
    return _description;
}

void Task::setDescription(const QString& description)
{
    if (description == _description)
        return;

    _description = description;

    emit descriptionChanged(_description);
}

Task::Status Task::getStatus() const
{
    return _status;
}

bool Task::isIdle() const
{
    return _status == Status::Idle;
}

bool Task::isRunning() const
{
    return _status == Status::Running;
}

bool Task::isFinished() const
{
    return _status == Status::Finished;
}

bool Task::isAborted() const
{
    return _status == Status::Aborted;
}

void Task::setStatus(const Status& status)
{
    if (status == _status)
        return;

    _status = status;

    emit statusChanged(_status);
}

void Task::setIdle()
{
    setStatus(Status::Idle);
}

void Task::setRunning()
{
    setStatus(Status::Running);
}

void Task::setFinished()
{
    setStatus(Status::Finished);
}

void Task::setAborted()
{
    setStatus(Status::Aborted);
    setProgress(0.f);
}

AbstractTaskHandler* Task::getHandler()
{
    return _handler;
}

void Task::setHandler(AbstractTaskHandler* handler)
{
    if (handler == _handler)
        return;

    _handler = handler;

    emit handlerChanged(_handler);
}

float Task::getProgress() const
{
    return _progress;
}

void Task::setProgress(float progress, const QString& subtaskDescription /*= ""*/)
{
    progress = std::clamp(progress, 0.f, 1.0f);

    if (progress == _progress)
        return;

    _progress = progress;

    emit progressChanged(_progress);

    if (!subtaskDescription.isEmpty())
        setCurrentSubtaskDescription(subtaskDescription);
}

void Task::setNumberOfSubtasks(std::uint32_t numberOfSubtasks)
{
    if (numberOfSubtasks == _subtasks.count())
        return;

    _subtasks.resize(numberOfSubtasks);
    _subtasksDescriptions.resize(numberOfSubtasks);

    emit subtasksChanged(_subtasks);
}

void Task::setSubtaskFinished(std::uint32_t subtaskIndex)
{
    if (subtaskIndex >= _subtasks.count())
        return;

    _subtasks.setBit(subtaskIndex, true);

    setProgress(static_cast<float>(_subtasks.count(true)) / static_cast<float>(_subtasks.count()), _subtasksDescriptions[subtaskIndex]);

    emit subtasksChanged(_subtasks);
}

void Task::setSubtasksDescriptions(const QStringList& subtasksDescriptions)
{
    if (subtasksDescriptions == _subtasksDescriptions)
        return;

    if (subtasksDescriptions.count() != _subtasks.count())
        setNumberOfSubtasks(subtasksDescriptions.count());

    _subtasksDescriptions = subtasksDescriptions;

    emit subtasksDescriptionsChanged(_subtasksDescriptions);
}

void Task::setSubtaskDescription(std::uint32_t subtaskIndex, const QString& subtaskDescription)
{
    if (_subtasksDescriptions.count() <= subtaskIndex)
        return;

    _subtasksDescriptions[subtaskIndex] = subtaskDescription;
}

QString Task::getCurrentSubtaskDescription() const
{
    return _currentSubtaskDescription;
}

void Task::setCurrentSubtaskDescription(const QString& currentSubtaskDescription)
{
    if (currentSubtaskDescription == _currentSubtaskDescription)
        return;

    _currentSubtaskDescription = currentSubtaskDescription;

    emit currentSubtaskDescriptionChanged(currentSubtaskDescription);
}

void Task::start(std::uint32_t numberOfItems)
{
    setNumberOfSubtasks(numberOfItems);
    setStatus(Status::Running);
}

}
