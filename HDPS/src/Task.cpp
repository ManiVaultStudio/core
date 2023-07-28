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
    _subtasksNames(),
    _progressDescription()
{
    tasks().addTask(this);

    QObject::connect(this, &QObject::destroyed, this, [this]() -> void {
        tasks().removeTask(this);
    });
}

Task::~Task()
{
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

    QCoreApplication::processEvents();
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

QString Task::getProgressText() const
{
    switch (getStatus())
    {
        case Task::Status::Idle:
            return "Idle";

        case Task::Status::Running:
            return QString("%1 %2%").arg(getProgressDescription().isEmpty() ? "" : QString("%1: ").arg(getProgressDescription()), QString::number(getProgress() * 100.f));

        case Task::Status::RunningIndeterminate:
            return getProgressDescription();

        case Task::Status::Finished:
            return "Finished";

        case Task::Status::Aborted:
            return "Aborted";

        default:
            break;
    }

    return {};
}

void Task::setSubtasks(std::uint32_t numberOfSubtasks)
{
    setProgressMode(ProgressMode::Subtasks);

    if (numberOfSubtasks == 0)
        return;

    if (numberOfSubtasks == _subtasks.count())
        return;

    _subtasks.resize(numberOfSubtasks);
    _subtasksNames.resize(numberOfSubtasks);
    
    emit subtasksChanged(_subtasks, _subtasksNames);
}

void Task::setSubtasks(const QStringList& subtasksNames)
{
    setProgressMode(ProgressMode::Subtasks);

    if (_subtasks.count() != subtasksNames.count())
        _subtasks.resize(subtasksNames.count());

    if (subtasksNames == _subtasksNames)
        return;

    _subtasksNames = subtasksNames;

    emit subtasksChanged(_subtasks, _subtasksNames);
}

void Task::addSubtasks(const QStringList& subtasksNames)
{
    if (subtasksNames.isEmpty())
        return;

    if (_progressMode != ProgressMode::Subtasks)
        return;

    _subtasksNames << subtasksNames;
    
    _subtasks.resize(_subtasksNames.count());

    emit subtasksChanged(_subtasks, _subtasksNames);

    updateProgress();
}

void Task::setSubtaskFinished(std::uint32_t subtaskIndex)
{
    if (_progressMode != ProgressMode::Subtasks)
        return;

    if (subtaskIndex >= _subtasks.count())
        return;

    _subtasks.setBit(subtaskIndex, true);

    updateProgress();

    emit subtasksChanged(_subtasks, _subtasksNames);

    const auto subtaskDescription = _subtasksNames[subtaskIndex];

    if (!subtaskDescription.isEmpty())
        setProgressDescription(subtaskDescription);
}

void Task::setSubtaskFinished(const QString& subtaskName)
{
    if (_progressMode != ProgressMode::Subtasks)
        return;

    const auto subtaskIndex = getSubtaskIndex(subtaskName);

    if (subtaskIndex < 0)
        return;

    setSubtaskFinished(subtaskIndex);
}

void Task::setSubtaskStarted(const QString& subtaskName)
{
    if (_progressMode != ProgressMode::Subtasks)
        return;

    const auto subtaskIndex = getSubtaskIndex(subtaskName);

    if (subtaskIndex < 0)
        return;

    emit progressDescriptionChanged(_subtasksNames[subtaskIndex]);
}

void Task::setSubtaskName(std::uint32_t subtaskIndex, const QString& subtaskName)
{
    if (_progressMode != ProgressMode::Subtasks)
        return;

    if (_subtasksNames.count() <= subtaskIndex)
        return;

    _subtasksNames[subtaskIndex] = subtaskName;
}

QStringList Task::getSubtasksNames() const
{
    return _subtasksNames;
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

    emit progressDescriptionChanged(_progressDescription);

    QCoreApplication::processEvents();
}

std::int32_t Task::getSubtaskIndex(const QString& subtaskName) const
{
    if (_progressMode != ProgressMode::Subtasks)
        return -1;

    if (!_subtasksNames.contains(subtaskName))
        return -1;

    return _subtasksNames.indexOf(subtaskName);
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

    QCoreApplication::processEvents();
}

}
