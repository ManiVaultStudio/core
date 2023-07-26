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
    { Task::Status::RunningIndeterminate, "Running Indeterminate" },
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
    _progressMode(ProgressMode::Manual),
    _progress(0.f),
    _subtasks(),
    _subtasksDescriptions(),
    _progressDescription()
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

bool Task::isRunningIndeterminate() const
{
    return _status == Status::RunningIndeterminate;
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

    updateProgress();

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

void Task::setRunningIndeterminate()
{
    setStatus(Status::RunningIndeterminate);
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

Task::ProgressMode Task::getProgressMode() const
{
    return _progressMode;
}

void Task::setProgressMode(const ProgressMode& progressMode)
{
    if (progressMode == _progressMode)
        return;

    _progressMode = progressMode;

    emit progressModeChanged(_progressMode);

    updateProgress();
}

float Task::getProgress() const
{
    return _progress;
}

void Task::setProgress(float progress, const QString& subtaskDescription /*= ""*/)
{
    if (_progressMode != ProgressMode::Manual)
        return;

    progress = std::clamp(progress, 0.f, 1.0f);

    if (progress == _progress)
        return;

    _progress = progress;

    updateProgress();

    if (!subtaskDescription.isEmpty())
        setProgressDescription(subtaskDescription);
}

void Task::setNumberOfSubtasks(std::uint32_t numberOfSubtasks)
{
    if (_progressMode != ProgressMode::Subtasks)
        return;

    if (numberOfSubtasks == 0)
        return;

    if (numberOfSubtasks == _subtasks.count())
        return;

    _subtasks.resize(numberOfSubtasks);
    _subtasksDescriptions.resize(numberOfSubtasks);
    
    setProgressMode(ProgressMode::Subtasks);

    emit subtasksChanged(_subtasks);
}

void Task::setSubtaskFinished(std::uint32_t subtaskIndex)
{
    if (_progressMode != ProgressMode::Subtasks)
        return;

    if (subtaskIndex >= _subtasks.count())
        return;

    _subtasks.setBit(subtaskIndex, true);

    updateProgress();

    emit subtasksChanged(_subtasks);

    const auto subtaskDescription = _subtasksDescriptions[subtaskIndex];

    if (!subtaskDescription.isEmpty())
        setProgressDescription(subtaskDescription);
}

void Task::setSubtasksDescriptions(const QStringList& subtasksDescriptions)
{
    if (_progressMode != ProgressMode::Subtasks)
        return;

    if (subtasksDescriptions == _subtasksDescriptions)
        return;

    if (subtasksDescriptions.count() != _subtasks.count())
        setNumberOfSubtasks(subtasksDescriptions.count());

    _subtasksDescriptions = subtasksDescriptions;

    emit subtasksDescriptionsChanged(_subtasksDescriptions);
}

void Task::setSubtaskDescription(std::uint32_t subtaskIndex, const QString& subtaskDescription)
{
    if (_progressMode != ProgressMode::Subtasks)
        return;

    if (_subtasksDescriptions.count() <= subtaskIndex)
        return;

    _subtasksDescriptions[subtaskIndex] = subtaskDescription;
}

QString Task::getProgressDescription() const
{
    return _progressDescription;
}

void Task::setProgressDescription(const QString& progressDescription)
{
    if (progressDescription == _progressDescription)
        return;

    _progressDescription = progressDescription;

    emit currentSubtaskDescriptionChanged(_progressDescription);
}

void Task::start(std::uint32_t numberOfItems)
{
    setNumberOfSubtasks(numberOfItems);
    setStatus(Status::Running);
}

void Task::updateProgress()
{
    switch (_progressMode) {
        case ProgressMode::Manual:
            break;

        case ProgressMode::Subtasks:
            _progress = _subtasks.isEmpty() ? 0.f : static_cast<float>(_subtasks.count(true)) / static_cast<float>(_subtasks.count());
            break;
    }

    switch (_status)
    {
        case Status::Undefined:
        case Status::Idle:
            _progress = 0.f;
            break;

        case Status::Running:
            break;

        case Status::RunningIndeterminate:
            _progress = 0.f;
            break;

        case Status::Finished:
            _progress = 1.f;
            break;

        case Status::Aborted:
            _progress = 0.f;
            break;
    }

    emit progressChanged(_progress);

    if (_status == Status::Finished || _status == Status::Aborted)
        setProgressDescription("");
}

}
