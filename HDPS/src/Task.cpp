// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Task.h"
#include "CoreInterface.h"

#ifdef _DEBUG
    #define TASK_VERBOSE
#endif

namespace hdps {

using namespace util;

QMap<Task::Status, QString> Task::statusNames = QMap<Status, QString>({
    { Task::Status::Undefined, "Undefined" },
    { Task::Status::Idle, "Idle" },
    { Task::Status::Running, "Running" },
    { Task::Status::RunningIndeterminate, "Running Indeterminate" },
    { Task::Status::Finished, "Finished" },
    { Task::Status::Aborting, "Aborting" },
    { Task::Status::AboutToBeAborted, "About to be aborted" },
    { Task::Status::Aborted, "Aborted" }
});

QMap<Task::ProgressMode, QString> Task::progressModeNames = QMap<ProgressMode, QString>({
    { Task::ProgressMode::Manual, "Manual" },
    { Task::ProgressMode::Subtasks, "Subtasks" },
    { Task::ProgressMode::Aggregate, "Aggregate" }
});

QMap<Task::Scope, QString> Task::scopeNames = QMap<Scope, QString>({
    { Task::Scope::Background, "Background" },
    { Task::Scope::Foreground, "Foreground" },
    { Task::Scope::Modal, "Modal" }
});

Task::Task(QObject* parent, const QString& name, const Scope& scope /*= Scope::Background*/, const Status& status /*= Status::Undefined*/, bool mayKill /*= false*/, AbstractTaskHandler* handler /*= nullptr*/) :
    QObject(parent),
    Serializable(name),
    _name(name),
    _description(),
    _icon(),
    _enabled(true),
    _visible(true),
    _status(status),
    _mayKill(mayKill),
    _handler(handler),
    _progressMode(ProgressMode::Manual),
    _scope(scope),
    _progress(0.f),
    _subtasks(),
    _subtasksNames(),
    _progressDescription(),
    _timers(),
    _parentTask(nullptr),
    _childTasks()
{
    if (core() != nullptr && core()->isInitialized())
        tasks().addTask(this);

    for (auto& timer : _timers)
        timer.setSingleShot(true);

    _timers[static_cast<int>(TimerType::ProgressChanged)].setInterval(TASK_UPDATE_TIMER_INTERVAL);
    _timers[static_cast<int>(TimerType::ProgressDescriptionChanged)].setInterval(TASK_UPDATE_TIMER_INTERVAL);
    _timers[static_cast<int>(TimerType::ToIdleWithDelay)].setInterval(TASK_DESCRIPTION_DISAPPEAR_INTERVAL);

    connect(&getTimer(TimerType::ProgressChanged), &QTimer::timeout, this, [this]() -> void {
        emit progressChanged(_progress);

        QCoreApplication::processEvents();
    });

    connect(&getTimer(TimerType::ProgressDescriptionChanged), &QTimer::timeout, this, [this]() -> void {
        emit progressDescriptionChanged(_progressDescription);

        QCoreApplication::processEvents();
    });

    connect(&getTimer(TimerType::ToIdleWithDelay), &QTimer::timeout, this, [this]() -> void {
        setIdle();
    });

    connect(this, &Task::privateSetParentTaskSignal, this, &Task::privateSetParentTask);
    connect(this, &Task::privateAddChildTaskSignal, this, &Task::privateAddChildTask);
    connect(this, &Task::privateRemoveChildTaskSignal, this, &Task::privateRemoveChildTask);
    connect(this, &Task::privateSetNameSignal, this, &Task::privateSetName);
    connect(this, &Task::privateSetDescriptionSignal, this, &Task::privateSetDescription);
    connect(this, &Task::privateSetIconSignal, this, &Task::privateSetIcon);
    connect(this, &Task::privateSetEnabledSignal, this, &Task::privateSetEnabled);
    connect(this, &Task::privateSetVisibleSignal, this, &Task::privateSetVisible);
    connect(this, &Task::privateSetStatusSignal, this, &Task::privateSetStatus);
    connect(this, &Task::privateSetIdleSignal, this, &Task::privateSetIdle);
    connect(this, &Task::privateSetRunningSignal, this, &Task::privateSetRunning);
    connect(this, &Task::privateSetRunningIndeterminateSignal, this, &Task::privateSetRunningIndeterminate);
    connect(this, qOverload<bool, std::uint32_t, QPrivateSignal>(&Task::privateSetFinishedSignal), this, qOverload<bool, std::uint32_t>(&Task::privateSetFinished));
    connect(this, qOverload<const QString&, bool, std::uint32_t, QPrivateSignal>(&Task::privateSetFinishedSignal), this, qOverload<const QString&, bool, std::uint32_t>(&Task::privateSetFinished));
    connect(this, &Task::privateSetAboutToBeAbortedSignal, this, &Task::privateSetAboutToBeAborted);
    connect(this, &Task::privateSetAbortingSignal, this, &Task::privateSetAborting);
    connect(this, &Task::privateSetAbortedSignal, this, &Task::privateSetAborted);
    connect(this, &Task::privateKillSignal, this, &Task::privateKill);
    connect(this, &Task::privateSetProgressModeSignal, this, &Task::privateSetProgressMode);
    connect(this, &Task::privateSetScopeSignal, this, &Task::privateSetScope);
    connect(this, &Task::privateSetProgressSignal, this, &Task::privateSetProgress);
    connect(this, qOverload<std::uint32_t, QPrivateSignal>(&Task::privateSetSubtasksSignal), this, qOverload<std::uint32_t>(&Task::privateSetSubtasks));
    connect(this, qOverload<const QStringList&, QPrivateSignal>(&Task::privateSetSubtasksSignal), this, qOverload<const QStringList&>(&Task::privateSetSubtasks));
    connect(this, qOverload<std::uint32_t, const QString&, QPrivateSignal>(&Task::privateSetSubtaskStartedSignal), this, qOverload<std::uint32_t, const QString&>(&Task::privateSetSubtaskStarted));
    connect(this, qOverload<const QString&, const QString&, QPrivateSignal>(&Task::privateSetSubtaskStartedSignal), this, qOverload<const QString&, const QString&>(&Task::privateSetSubtaskStarted));
    connect(this, qOverload<std::uint32_t, const QString&, QPrivateSignal>(&Task::privateSetSubtaskFinishedSignal), this, qOverload<std::uint32_t, const QString&>(&Task::privateSetSubtaskFinished));
    connect(this, qOverload<const QString&, const QString&, QPrivateSignal>(&Task::privateSetSubtaskFinishedSignal), this, qOverload<const QString&, const QString&>(&Task::privateSetSubtaskFinished));
    connect(this, &Task::privateSetSubtaskNameSignal, this, &Task::privateSetSubtaskName);
    connect(this, &Task::privateSetProgressDescriptionSignal, this, &Task::privateSetProgressDescription);
    connect(this, &Task::privateSetProgressTextFormatterSignal, this, &Task::privateSetProgressTextFormatter);
}

Task::~Task()
{
    if (hasParentTask())
        getParentTask()->removeChildTask(this);

    for (auto& timer : _timers)
        timer.stop();

    if (core() != nullptr && core()->isInitialized())
        tasks().removeTask(this);
}

QString Task::getTypeName(bool humanFriendly /*= true*/) const
{
    auto typeString = QString(metaObject()->className());

    if (humanFriendly)
        typeString.replace("hdps::", "");

    return typeString;
}

Task* Task::getParentTask()
{
    return _parentTask;
}

void Task::setParentTask(Task* parentTask)
{
    emit privateSetParentTaskSignal(parentTask, QPrivateSignal());
}

bool Task::hasParentTask()
{
    return getParentTask() != nullptr;
}

Task::TasksPtrs Task::getChildTasks(bool recursively /*= false*/) const
{
    auto childTasks = _childTasks;

    if (recursively)
        for (auto childTask : _childTasks)
            childTasks << childTask->getChildTasks(recursively);

    return _childTasks;
}

void Task::addChildTask(Task* childTask)
{
    emit privateAddChildTaskSignal(childTask, QPrivateSignal());
}

void Task::removeChildTask(Task* childTask)
{
    emit privateRemoveChildTaskSignal(childTask, QPrivateSignal());
}

QString Task::getName() const
{
    return _name;
}

void Task::setName(const QString& name)
{
    emit privateSetNameSignal(name, QPrivateSignal());
}

QString Task::getDescription() const
{
    return _description;
}

void Task::setDescription(const QString& description)
{
    emit privateSetDescriptionSignal(description, QPrivateSignal());
}

QIcon Task::getIcon() const
{
    return _icon;
}

void Task::setIcon(const QIcon& icon)
{
    emit privateSetIconSignal(icon, QPrivateSignal());
}

bool Task::getEnabled() const
{
    return _enabled;
}

void Task::setEnabled(bool enabled)
{
    emit privateSetEnabledSignal(enabled, QPrivateSignal());
}

bool Task::getVisible() const
{
    return _visible;
}

void Task::setVisible(bool visible)
{
    emit privateSetVisibleSignal(visible, QPrivateSignal());
}

bool Task::getMayKill() const
{
    return _mayKill;
}

void Task::setMayKill(bool mayKill)
{
    if (mayKill == _mayKill)
        return;

    _mayKill = mayKill;

    emit mayKillChanged(getMayKill());
    emit isKillableChanged(isKillable());
}

bool Task::isKillable() const
{
    if (!(isRunning() || isRunningIndeterminate()))
        return false;

    return getMayKill();
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

bool Task::isAboutToBeAborted() const
{
    return _status == Status::AboutToBeAborted;
}

bool Task::isAborting() const
{
    return _status == Status::Aborting;
}

bool Task::isAborted() const
{
    return _status == Status::Aborted;
}

void Task::setStatus(const Status& status)
{
    emit privateSetStatusSignal(status, QPrivateSignal());
}

void Task::setIdle()
{
    emit privateSetIdleSignal(QPrivateSignal());
}

void Task::setRunning()
{
    emit privateSetRunningSignal(QPrivateSignal());
}

void Task::setRunningIndeterminate()
{
    emit privateSetRunningIndeterminateSignal(QPrivateSignal());
}

void Task::setFinished(bool toIdleWithDelay /*= true*/, std::uint32_t delay /*= TASK_DESCRIPTION_DISAPPEAR_INTERVAL*/)
{
    emit privateSetFinishedSignal(hasParentTask() ? false : toIdleWithDelay, delay, QPrivateSignal());
}

void Task::setFinished(const QString& progressDescription, bool toIdleWithDelay /*= true*/, std::uint32_t delay /*= TASK_DESCRIPTION_DISAPPEAR_INTERVAL*/)
{
    emit privateSetFinishedSignal(progressDescription, hasParentTask() ? false : toIdleWithDelay, delay, QPrivateSignal());
}

void Task::setAboutToBeAborted()
{
    emit privateSetAboutToBeAbortedSignal(QPrivateSignal());
}

void Task::setAborting()
{
    emit privateSetAbortingSignal(QPrivateSignal());
}

void Task::setAborted()
{
    emit privateSetAbortedSignal(QPrivateSignal());
}

void Task::kill(bool recursive /*= true*/)
{
    emit privateKillSignal(recursive, QPrivateSignal());
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
    emit privateSetProgressModeSignal(progressMode, QPrivateSignal());
}

Task::Scope Task::getScope() const
{
    return _scope;
}

void Task::setScope(const Scope& scope)
{
    emit privateSetScopeSignal(scope, QPrivateSignal());
}

float Task::getProgress() const
{
    return _progress;
}

void Task::setProgress(float progress, const QString& subtaskDescription /*= ""*/)
{
    emit privateSetProgressSignal(progress, subtaskDescription, QPrivateSignal());
}

QString Task::getProgressText() const
{
    if (_progressTextFormatter)
        return const_cast<Task*>(this)->_progressTextFormatter(*const_cast<Task*>(this));

    switch (getStatus())
    {
        case Task::Status::Idle:
            return "Idle";

        case Task::Status::Running:
        {
            if (_progress == 0.f)
                return _name;
            else
                return QString("%1 %2%").arg(getProgressDescription().isEmpty() ? "" : QString("%1").arg(getProgressDescription()), QString::number(getProgress() * 100.f, 'f', 1));
        }

        case Task::Status::RunningIndeterminate:
            return QString("%1: %2").arg(getName(), getProgressDescription());

        case Task::Status::Finished:
            return QString("%1 finished").arg(getName());

        case Task::Status::AboutToBeAborted:
            return QString("%1 is about to be aborted").arg(getName());

        case Task::Status::Aborting:
            return QString("%1 is aborting").arg(getName());

        case Task::Status::Aborted:
            return QString("%1 is aborted").arg(getName());

        default:
            break;
    }

    return {};
}

void Task::setSubtasks(std::uint32_t numberOfSubtasks)
{
    emit privateSetSubtasksSignal(numberOfSubtasks, QPrivateSignal());
}

void Task::setSubtasks(const QStringList& subtasksNames)
{
    emit privateSetSubtasksSignal(subtasksNames, QPrivateSignal());
}

void Task::setSubtaskStarted(std::uint32_t subtaskIndex, const QString& progressDescription /*= QString()*/)
{
    emit privateSetSubtaskStartedSignal(subtaskIndex, progressDescription, QPrivateSignal());
}

void Task::setSubtaskStarted(const QString& subtaskName, const QString& progressDescription /*= QString()*/)
{
    emit privateSetSubtaskStartedSignal(subtaskName, progressDescription, QPrivateSignal());
}

void Task::setSubtaskFinished(std::uint32_t subtaskIndex, const QString& progressDescription /*= QString()*/)
{
    emit privateSetSubtaskFinishedSignal(subtaskIndex, progressDescription, QPrivateSignal());
}

void Task::setSubtaskFinished(const QString& subtaskName, const QString& progressDescription /*= QString()*/)
{
    emit privateSetSubtaskFinishedSignal(subtaskName, progressDescription, QPrivateSignal());
}

void Task::setSubtaskName(std::uint32_t subtaskIndex, const QString& subtaskName)
{
    emit privateSetSubtaskNameSignal(subtaskIndex, subtaskName, QPrivateSignal());
}

QStringList Task::getSubtasksNames() const
{
    return _subtasksNames;
}

QString Task::getProgressDescription() const
{
    return _progressDescription;
}

void Task::setProgressDescription(const QString& progressDescription, std::uint32_t clearDelay /*= 0*/)
{
    emit privateSetProgressDescriptionSignal(progressDescription, clearDelay, QPrivateSignal());
}

void Task::setProgressTextFormatter(const ProgressTextFormatter& progressTextFormatter)
{
    emit privateSetProgressTextFormatterSignal(progressTextFormatter, QPrivateSignal());
}

std::int32_t Task::getSubtaskIndex(const QString& subtaskName) const
{
    if (_progressMode != ProgressMode::Subtasks)
        return -1;

    if (!_subtasksNames.contains(subtaskName))
        return -1;

    return _subtasksNames.indexOf(subtaskName);
}

void Task::computeProgress()
{
//#ifdef TASK_VERBOSE
//    qDebug() << __FUNCTION__ << getName();
//#endif

    switch (_progressMode) {
        case ProgressMode::Manual:
            break;

        case ProgressMode::Subtasks:
        {
            _progress = _subtasks.isEmpty() ? 0.f : static_cast<float>(_subtasks.count(true)) / static_cast<float>(_subtasks.size() / 8);
            break;
        }

        case ProgressMode::Aggregate:
        {
            const auto childTasks = getChildTasks();

            std::int32_t numberOfEnabledChildTasks = 0;

            auto accumulatedProgress = std::accumulate(childTasks.begin(), childTasks.end(), 0.f, [&numberOfEnabledChildTasks](float sum, Task* childTask) -> float {
                if (!childTask->getEnabled())
                    return sum;

                numberOfEnabledChildTasks++;

                return sum + childTask->getProgress();
            });

            _progress = accumulatedProgress / static_cast<float>(numberOfEnabledChildTasks);

            break;
        }
    }

    switch (_status)
    {
        case Status::Undefined:
        case Status::Idle:
        {
            if (getProgressMode() != ProgressMode::Aggregate)
                _progress = 0.f;

            break;
        }

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

    if (!getTimer(TimerType::ProgressChanged).isActive()) {
        emit progressChanged(_progress);

        QCoreApplication::processEvents();

        getTimer(TimerType::ProgressChanged).start();
    }

    if (_status == Status::Finished || _status == Status::Aborted)
        privateSetProgressDescription("", TASK_DESCRIPTION_DISAPPEAR_INTERVAL);
}

QTimer& Task::getTimer(const TimerType& timerType)
{
    return _timers[static_cast<int>(timerType)];
}

void Task::registerChildTask(Task* childTask)
{
    Q_ASSERT(childTask != nullptr);

    if (childTask == nullptr)
        return;

    connect(childTask, &Task::statusChanged, this, [this](const Status& previousStatus, const Status& status) -> void {
        if (getProgressMode() != ProgressMode::Aggregate)
            return;

        updateAggregateStatus();
        computeProgress();
    });

    connect(childTask, &Task::progressChanged, this, [this](float progress) -> void {
        if (getProgressMode() != ProgressMode::Aggregate)
            return;

        computeProgress();
    });

    connect(childTask, &Task::progressDescriptionChanged, this, [this](const QString& progressDescription) -> void {
        if (getProgressMode() != ProgressMode::Aggregate)
            return;

        auto combinedProgressDescription = QString("%1: %2").arg(getName(), progressDescription);

        /*
        if (!hasParentTask()) {
            auto segments = combinedProgressDescription.split(": ");

            segments.removeLast();

            combinedProgressDescription = segments.join(": ");
        }
        */

        privateSetProgressDescription(combinedProgressDescription);
    });

    updateAggregateStatus();
}

void Task::unregisterChildTask(Task* childTask)
{
    Q_ASSERT(childTask != nullptr);

    if (childTask == nullptr)
        return;

    disconnect(childTask, &Task::statusChanged, this, nullptr);
    disconnect(childTask, &Task::progressChanged, this, nullptr);
    disconnect(childTask, &Task::progressDescriptionChanged, this, nullptr);
}

void Task::updateAggregateStatus()
{
    if (_progressMode != ProgressMode::Aggregate)
        return;

    const auto childTasks = getChildTasks();

    const auto numberOfEnabledChildTasks = std::accumulate(childTasks.begin(), childTasks.end(), 0.f, [](std::size_t count, Task* childTask) -> std::size_t {
        if (!childTask->getEnabled())
            return count;

        return count + 1;
    });

    const auto countStatus = [&childTasks](const Status& status) -> std::size_t {
        return std::accumulate(childTasks.begin(), childTasks.end(), 0.f, [status](std::size_t count, Task* childTask) -> std::size_t {
            if (!childTask->getEnabled())
                return count;

            return count + (childTask->getStatus() == status ? 1 : 0);
        });
    };

    if (countStatus(Status::Running) >= 1 || countStatus(Status::RunningIndeterminate) >= 1)
        privateSetRunning();

    if (countStatus(Status::Idle) == childTasks.count())
        privateSetProgress(0.f);

    if (countStatus(Status::Finished) == numberOfEnabledChildTasks) {
        auto tasksToSetToIdle = getChildTasks();

        std::reverse(tasksToSetToIdle.begin(), tasksToSetToIdle.end());

        for (auto taskToSetToIdle : tasksToSetToIdle)
            taskToSetToIdle->setIdle();

        privateSetFinished(!hasParentTask());
    }
}

void Task::privateSetParentTask(Task* parentTask)
{
    try {
        const auto previousParentTask = _parentTask;

        if (parentTask == _parentTask)
            return;

        if (parentTask && (parentTask->getTypeName() != getTypeName()))
            throw std::runtime_error(QString("Type mismatch: child type is %1 and parent type is %2").arg(getTypeName(), parentTask->getTypeName()).toStdString());

        _parentTask = parentTask;

        if (previousParentTask)
            previousParentTask->removeChildTask(this);

        if (_parentTask)
            _parentTask->addChildTask(this);

        emit parentTaskChanged(previousParentTask, _parentTask);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to set parent task for %1").arg(getName()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to set parent task for %1").arg(getName()));
    }
}

void Task::privateAddChildTask(Task* childTask)
{
    try {
        Q_ASSERT(childTask != nullptr);

        if (childTask == nullptr)
            throw std::runtime_error("Supplied task pointer is a nullptr");

        if (_childTasks.contains(childTask))
            throw std::runtime_error(QString("%1 already is a child of %2").arg(childTask->getName(), getName()).toStdString());

        _childTasks << childTask;

#ifdef TASK_VERBOSE
        qDebug() << "Child task" << childTask->getName() << "added to" << getName();
#endif

        setProgressMode(ProgressMode::Aggregate);
        registerChildTask(childTask);
        updateAggregateStatus();
        computeProgress();

        emit childTaskAdded(childTask);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to add child task to %1").arg(getName()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to add child task to %1").arg(getName()));
    }
}

void Task::privateRemoveChildTask(Task* childTask)
{
    try {
        Q_ASSERT(childTask != nullptr);

        if (childTask == nullptr)
            throw std::runtime_error("Supplied task pointer is a nullptr");

        if (_childTasks.contains(childTask))
            throw std::runtime_error(QString("%1 is not a child of %2").arg(childTask->getName(), getName()).toStdString());

        emit childTaskAboutToBeRemoved(childTask);
        {
            _childTasks.removeOne(childTask);

#ifdef TASK_VERBOSE
            qDebug() << "Child task" << childTask->getName() << "removed from" << getName();
#endif

            unregisterChildTask(childTask);
            updateAggregateStatus();
            computeProgress();
        }
        emit childTaskRemoved(childTask);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to remove child task from %1").arg(getName()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to remove child task from %1").arg(getName()));
    }
}

void Task::privateSetName(const QString& name)
{
    if (name == _name)
        return;

    _name = name;

    emit nameChanged(_name);
}

void Task::privateSetDescription(const QString& description)
{
    if (description == _description)
        return;

    _description = description;

    emit descriptionChanged(_description);
}

void Task::privateSetIcon(const QIcon& icon)
{
    _icon = icon;

    emit iconChanged(_icon);
}

void Task::privateSetEnabled(bool enabled)
{
    if (enabled == _enabled)
        return;

    _enabled = enabled;

    emit enabledChanged(_enabled);
}

void Task::privateSetVisible(bool visible)
{
    if (visible == _visible)
        return;

    _visible = visible;

    emit visibileChanged(_visible);
}

void Task::privateSetStatus(const Status& status)
{
    if (status == _status)
        return;

    const auto previousStatus = _status;

    _status = status;

    computeProgress();

    emit statusChanged(previousStatus, _status);

    switch (_status)
    {
        case Task::Status::Idle:
            emit idle();
            break;

        case Task::Status::Running:
            emit running();
            break;

        case Task::Status::RunningIndeterminate:
            emit runningIndeterminate();
            break;

        case Task::Status::Finished:
            emit finished();
            break;

        case Task::Status::AboutToBeAborted:
            emit isAboutToBeAborted();
            break;

        case Task::Status::Aborting:
            emit isAborting();
            break;

        case Task::Status::Aborted:
            emit isAborted();
            break;

        default:
            break;
    }

    emit isKillableChanged(isKillable());

    QCoreApplication::processEvents();
}

void Task::privateSetIdle()
{
    privateSetStatus(Status::Idle);

    privateSetProgressDescription("Idle", TASK_DESCRIPTION_DISAPPEAR_INTERVAL);
}

void Task::privateSetRunning()
{
    privateSetStatus(Status::Running);
}

void Task::privateSetRunningIndeterminate()
{
    privateSetStatus(Status::RunningIndeterminate);
}

void Task::privateSetFinished(bool toIdleWithDelay, std::uint32_t delay)
{
    privateSetStatus(Status::Finished);

    privateSetProgressDescription("Finished", TASK_DESCRIPTION_DISAPPEAR_INTERVAL);

    if (toIdleWithDelay) {
        getTimer(TimerType::ToIdleWithDelay).setInterval(delay);
        getTimer(TimerType::ToIdleWithDelay).start();
    }
}

void Task::privateSetFinished(const QString& progressDescription, bool toIdleWithDelay, std::uint32_t delay)
{
    privateSetStatus(Status::Finished);

    privateSetProgressDescription(progressDescription, TASK_DESCRIPTION_DISAPPEAR_INTERVAL);

    if (toIdleWithDelay) {
        getTimer(TimerType::ToIdleWithDelay).setInterval(delay);
        getTimer(TimerType::ToIdleWithDelay).start();
    }
}

void Task::privateSetAboutToBeAborted()
{
    emit isAboutToBeAborted();
}

void Task::privateSetAborting()
{
    if (!getMayKill())
        return;

    privateSetStatus(Status::Aborting);
    privateSetProgressDescription("Aborting...");

    emit isAborting();
}

void Task::privateSetAborted()
{
    if (!getMayKill())
        return;

    privateSetStatus(Status::Aborted);
    privateSetProgressDescription("Aborted", TASK_DESCRIPTION_DISAPPEAR_INTERVAL);

    setProgress(0.f);

    emit isAborted();
}

void Task::privateKill(bool recursive /*= true*/)
{
    auto tasksToKill = getChildTasks();

    std::reverse(tasksToKill.begin(), tasksToKill.end());

    for (auto taskToKill : tasksToKill)
        taskToKill->kill();

    if (getMayKill()) {
#ifdef TASK_VERBOSE
        qDebug() << __FUNCTION__ << getName();
#endif

        privateSetAboutToBeAborted();
        privateSetAborting();
        {
            emit requestAbort();
        }
        privateSetAborted();
    }
}

void Task::privateSetProgressMode(const ProgressMode& progressMode)
{
    if (progressMode == _progressMode)
        return;

    _progressMode = progressMode;

    emit progressModeChanged(_progressMode);

    if (_progressMode == ProgressMode::Aggregate)
        privateSetProgressDescription(getName());

    computeProgress();
}

void Task::privateSetScope(const Scope& scope)
{
    if (scope == _scope)
        return;

    _scope = scope;

    emit scopeChanged(_scope);
}

void Task::privateSetProgress(float progress, const QString& subtaskDescription /*= ""*/)
{
    if (_progressMode != ProgressMode::Manual)
        return;

    progress = std::clamp(progress, 0.f, 1.f);

    if (progress == _progress)
        return;

    _progress = progress;

    if (!subtaskDescription.isEmpty())
        privateSetProgressDescription(subtaskDescription);

    QCoreApplication::processEvents();
}

void Task::privateSetSubtasks(std::uint32_t numberOfSubtasks)
{
    privateSetProgressMode(ProgressMode::Subtasks);

    if (numberOfSubtasks == 0)
        return;

    _subtasks.clear();

    _subtasks.resize(numberOfSubtasks);
    _subtasksNames.resize(numberOfSubtasks);

    emit subtasksChanged(_subtasks, _subtasksNames);

    computeProgress();

    QCoreApplication::processEvents();
}

void Task::privateSetSubtasks(const QStringList& subtasksNames)
{
    privateSetProgressMode(ProgressMode::Subtasks);

    if (_subtasks.count() != subtasksNames.count())
        _subtasks.resize(subtasksNames.count() * 8);

    if (subtasksNames == _subtasksNames)
        return;

    _subtasksNames = subtasksNames;

    emit subtasksChanged(_subtasks, _subtasksNames);

    computeProgress();

    QCoreApplication::processEvents();
}

void Task::privateSetSubtaskStarted(std::uint32_t subtaskIndex, const QString& progressDescription)
{
    if (_progressMode != ProgressMode::Subtasks)
        return;

    if (subtaskIndex >= _subtasks.count())
        return;

    const auto subtaskName = _subtasksNames[subtaskIndex];

    if (progressDescription.isEmpty())
        privateSetProgressDescription(subtaskName);
    else
        privateSetProgressDescription(progressDescription);

    QCoreApplication::processEvents();
}

void Task::privateSetSubtaskStarted(const QString& subtaskName, const QString& progressDescription)
{
    if (_progressMode != ProgressMode::Subtasks)
        return;

    const auto subtaskIndex = getSubtaskIndex(subtaskName);

    if (subtaskIndex < 0)
        return;

    if (progressDescription.isEmpty())
        privateSetProgressDescription(subtaskName);
    else
        privateSetProgressDescription(progressDescription);

    QCoreApplication::processEvents();
}

void Task::privateSetSubtaskFinished(std::uint32_t subtaskIndex, const QString& progressDescription)
{
    if (_progressMode != ProgressMode::Subtasks)
        return;

    if (subtaskIndex >= _subtasks.count())
        return;

    _subtasks.setBit(subtaskIndex, true);

    computeProgress();

    const auto subtaskName = _subtasksNames[subtaskIndex];

    if (!progressDescription.isEmpty()) {
        privateSetProgressDescription(progressDescription);
    }
    else {
        if (!subtaskName.isEmpty())
            privateSetProgressDescription(subtaskName);
    }

    emit subtaskFinished(subtaskName);

    QCoreApplication::processEvents();
}

void Task::privateSetSubtaskFinished(const QString& subtaskName, const QString& progressDescription /*= QString()*/)
{
    if (_progressMode != ProgressMode::Subtasks)
        return;

    const auto subtaskIndex = getSubtaskIndex(subtaskName);

    if (subtaskIndex < 0)
        return;

    privateSetSubtaskFinished(subtaskIndex, progressDescription);
}

void Task::privateSetSubtaskName(std::uint32_t subtaskIndex, const QString& subtaskName)
{
    if (_progressMode != ProgressMode::Subtasks)
        return;

    if (_subtasksNames.count() <= subtaskIndex)
        return;

    _subtasksNames[subtaskIndex] = subtaskName;
}

void Task::privateSetProgressDescription(const QString& progressDescription, std::uint32_t clearDelay /*= 0*/)
{
    if (progressDescription == _progressDescription)
        return;

    _progressDescription = progressDescription;

    if (!getTimer(TimerType::ProgressDescriptionChanged).isActive()) {
        emit progressDescriptionChanged(_progressDescription);

        QCoreApplication::processEvents();

        getTimer(TimerType::ProgressDescriptionChanged).start();
    }

    if (clearDelay > 0) {
        QTimer::singleShot(clearDelay, this, [this]() -> void {
            privateSetProgressDescription("");
        });
    }
}

void Task::privateSetProgressTextFormatter(const ProgressTextFormatter& progressTextFormatter)
{
    _progressTextFormatter = progressTextFormatter;
}

}
