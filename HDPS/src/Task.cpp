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

QMap<Task::GuiScope, QString> Task::guiScopeNames = QMap<GuiScope, QString>({
    { Task::GuiScope::None, "None" },
    { Task::GuiScope::Background, "Background" },
    { Task::GuiScope::Foreground, "Foreground" },
    { Task::GuiScope::Modal, "Modal" }
});

Task::Task(QObject* parent, const QString& name, const GuiScope& guiScope /*= GuiScope::None*/, const Status& status /*= Status::Undefined*/, bool mayKill /*= false*/, AbstractTaskHandler* handler /*= nullptr*/) :
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
    _guiScope(guiScope),
    _progress(0.f),
    _subtasks(),
    _subtasksNames(),
    _progressDescription(),
    _timers(),
    _parentTask(nullptr),
    _childTasks(),
    _progressText(),
    _progressTextFormatter()
{
    if (core() != nullptr)
        tasks().addTask(this);
    else
        qDebug() << "Cannot add task to the manager because the core has not been initialized yet.";

    for (auto& timer : _timers)
        timer.setSingleShot(true);

    _timers[static_cast<int>(TimerType::ProgressChanged)].setInterval(TASK_UPDATE_TIMER_INTERVAL);
    _timers[static_cast<int>(TimerType::ProgressDescriptionChanged)].setInterval(TASK_UPDATE_TIMER_INTERVAL);
    _timers[static_cast<int>(TimerType::ProgressTextChanged)].setInterval(TASK_UPDATE_TIMER_INTERVAL);
    _timers[static_cast<int>(TimerType::ToIdleWithDelay)].setInterval(TASK_DESCRIPTION_DISAPPEAR_INTERVAL);

    connect(&getTimer(TimerType::ProgressChanged), &QTimer::timeout, this, [this]() -> void {
        emit progressChanged(getProgress());

        QCoreApplication::processEvents();
    });

    connect(&getTimer(TimerType::ProgressDescriptionChanged), &QTimer::timeout, this, [this]() -> void {
        emit progressDescriptionChanged(getProgressDescription());

        QCoreApplication::processEvents();
    });

    connect(&getTimer(TimerType::ProgressTextChanged), &QTimer::timeout, this, [this]() -> void {
        emit progressTextChanged(getProgressText());

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
    connect(this, &Task::privateSetGuiScopeSignal, this, &Task::privateSetGuiScope);
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

    for (auto childTask : _childTasks)
        childTask->setParentTask(nullptr);

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

Task::TasksPtrs Task::getChildTasks(bool recursively /*= false*/, bool enabledOnly /*= true*/) const
{
    TasksPtrs childTasks;
    
    for (auto childTask : _childTasks) {
        if (enabledOnly && !childTask->getEnabled())
            continue;

        childTasks << childTask;
    }

    if (recursively)
        for (auto childTask : _childTasks)
            childTasks << childTask->getChildTasks(recursively, enabledOnly);

    return childTasks;
}

Task::TasksPtrs Task::getChildTasksForStatuses(bool recursively /*= false*/, bool enabledOnly /*= true*/, const Statuses& statuses /*= Statuses()*/) const
{
    auto childTasks = getChildTasks(recursively, enabledOnly);

    childTasks.erase(std::find_if(childTasks.begin(), childTasks.end(), [&statuses](Task* task) -> bool {
        return !statuses.contains(task->getStatus());
    }), childTasks.end());

    return childTasks;
}

Task::TasksPtrs Task::getChildTasksForGuiScopes(bool recursively /*= false*/, bool enabledOnly /*= true*/, const GuiScopes& guiScopes /*= GuiScopes()*/) const
{
    auto childTasks = getChildTasks(recursively, enabledOnly);

    childTasks.erase(std::find_if(childTasks.begin(), childTasks.end(), [&guiScopes](Task* task) -> bool {
        return !guiScopes.contains(task->getGuiScope());
    }), childTasks.end());

    return childTasks;
}

Task::TasksPtrs Task::getChildTasksForGuiScopesAndStatuses(bool recursively /*= false*/, bool enabledOnly /*= true*/, const GuiScopes& guiScopes /*= GuiScopes()*/, const Statuses& statuses /*= Statuses()*/) const
{
    auto childTasks = getChildTasks(recursively, enabledOnly);

    childTasks.erase(std::find_if(childTasks.begin(), childTasks.end(), [&guiScopes, &statuses](Task* task) -> bool {
        return !guiScopes.contains(task->getGuiScope()) || !statuses.contains(task->getStatus());
    }), childTasks.end());

    return childTasks;
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

Task::GuiScope Task::getGuiScope() const
{
    return _guiScope;
}

void Task::setGuiScope(const GuiScope& guiScope)
{
    emit privateSetGuiScopeSignal(guiScope, QPrivateSignal());
}

float Task::getProgress() const
{
    return _progress;
}

void Task::setProgress(float progress, const QString& subtaskDescription /*= ""*/)
{
    emit privateSetProgressSignal(progress, subtaskDescription, QPrivateSignal());
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

QString Task::getProgressText() const
{
    return _progressText;
}

void Task::setProgressTextFormatter(const ProgressTextFormatter& progressTextFormatter)
{
    emit privateSetProgressTextFormatterSignal(progressTextFormatter, QPrivateSignal());
}

QString Task::getStandardProgressText() const
{
    switch (getStatus())
    {
        case Task::Status::Undefined:
            return "Undefined";

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
            return "Finished";

        case Task::Status::AboutToBeAborted:
            return "About to be aborted";

        case Task::Status::Aborting:
            return "Aborting";

        case Task::Status::Aborted:
            return "aborted";

        default:
            break;
    }

    return {};
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
            const auto childTasks = getChildTasksForStatuses(false, true, { Status::Undefined, Status::Idle, Status::Running, Status::RunningIndeterminate });

            //QStringList childTasksNames;

            //for (auto childTask : getChildTasks())
            //    childTasksNames << childTask->getName() << statusNames[childTask->getStatus()];

            //if (getName() == "Load workspace")
            //    qDebug() << getName() << childTasksNames;

            auto accumulatedProgress = std::accumulate(childTasks.begin(), childTasks.end(), 0.f, [](float sum, Task* childTask) -> float {
                return sum + childTask->getProgress();
            });

            _progress = accumulatedProgress / static_cast<float>(childTasks.size());
            
            break;
        }
    }

    switch (_status)
    {
        case Status::Undefined:
        case Status::Idle:
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

    if (!getTimer(TimerType::ProgressChanged).isActive()) {
        emit progressChanged(_progress);

        QCoreApplication::processEvents();

        getTimer(TimerType::ProgressChanged).start();
    }
}

void Task::updateProgressText()
{
    const auto previousProgressText = _progressText;

    if (_progressTextFormatter)
        _progressText = const_cast<Task*>(this)->_progressTextFormatter(*const_cast<Task*>(this));
    else
        _progressText = getStandardProgressText();

    if (_progressText == previousProgressText)
        return;

    if (!getTimer(TimerType::ProgressTextChanged).isActive()) {
        emit progressTextChanged(_progressText);

        QCoreApplication::processEvents();

        getTimer(TimerType::ProgressTextChanged).start();
    }
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

#ifdef TASK_VERBOSE
    qDebug() << "*****"  << __FUNCTION__ << getName() << childTask->getName();
#endif

    connect(childTask, &Task::statusChanged, this, [this](const Status& previousStatus, const Status& status) -> void {
        if (getProgressMode() != ProgressMode::Aggregate)
            return;

        updateAggregateStatus();
        updateProgress();
    });

    connect(childTask, &Task::progressChanged, this, [this](float progress) -> void {
        if (getProgressMode() != ProgressMode::Aggregate)
            return;

        updateProgress();
    });

    connect(childTask, &Task::progressDescriptionChanged, this, [this](const QString& progressDescription) -> void {
        if (getProgressMode() != ProgressMode::Aggregate)
            return;

        auto combinedProgressDescription = progressDescription;// QString("%1: %2").arg(getName(), progressDescription);

        QStringList childTasksNames;

        for (auto childTask : getChildTasks())
            childTasksNames << childTask->getName() << statusNames[childTask->getStatus()];
        
        if (getName() == "Loading Project")
            qDebug() << "*********************" << childTasksNames;

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

    const auto childTasks           = getChildTasks();
    const auto numberOfChildTasks   = childTasks.size();

    const auto countStatus = [this, &childTasks](const Status& status) -> std::size_t {
        return getChildTasksForStatuses(false, true, { status }).count();
    };
    
    if (countStatus(Status::Running) >= 1 || countStatus(Status::RunningIndeterminate) >= 1)
        privateSetRunning();

    if (countStatus(Status::Idle) == numberOfChildTasks)
        privateSetProgress(0.f);

    if (countStatus(Status::Finished) >= 1 && countStatus(Status::Running) == 0 && countStatus(Status::RunningIndeterminate) == 0) {
        auto tasksToSetToIdle = getChildTasksForStatuses(false, true, { Status::Finished });

        std::reverse(tasksToSetToIdle.begin(), tasksToSetToIdle.end());

        //for (auto taskToSetToIdle : tasksToSetToIdle)
        //    taskToSetToIdle->setIdle();

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
            qDebug() << QString("Type mismatch: child type is %1 and parent type is %2").arg(getTypeName(), parentTask->getTypeName());

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
        updateProgress();

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

        if (!_childTasks.contains(childTask))
            throw std::runtime_error(QString("%1 is not a child of %2").arg(childTask->getName(), getName()).toStdString());

        emit childTaskAboutToBeRemoved(childTask);
        {
            _childTasks.removeOne(childTask);

#ifdef TASK_VERBOSE
            qDebug() << "Child task" << childTask->getName() << "removed from" << getName();
#endif

            unregisterChildTask(childTask);
            updateAggregateStatus();
            updateProgress();
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

    updateProgress();
    updateProgressText();

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
    privateSetProgress(0.f);

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

    updateProgress();
}

void Task::privateSetGuiScope(const GuiScope& guiScope)
{
    if (guiScope == _guiScope)
        return;

    _guiScope = guiScope;

    emit guiScopeChanged(_guiScope);
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

    updateProgressText();
}

void Task::privateSetSubtasks(std::uint32_t numberOfSubtasks)
{
    privateSetProgressMode(ProgressMode::Subtasks);

    if (numberOfSubtasks == 0)
        return;

    const auto numberOfBits = numberOfSubtasks * 8;

    _subtasks.clear();
    _subtasks.resize(numberOfBits);
    _subtasks.fill(false, numberOfBits);

    _subtasksNames.resize(numberOfSubtasks);

    emit subtasksChanged(_subtasks, _subtasksNames);

    updateProgress();
}

void Task::privateSetSubtasks(const QStringList& subtasksNames)
{
    privateSetProgressMode(ProgressMode::Subtasks);

    const auto numberOfBits = subtasksNames.count() * 8;

    _subtasks.resize(numberOfBits);
    _subtasks.fill(false, numberOfBits);

    _subtasksNames = subtasksNames;

    emit subtasksChanged(_subtasks, _subtasksNames);

    updateProgress();
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
}

void Task::privateSetSubtaskFinished(std::uint32_t subtaskIndex, const QString& progressDescription)
{
    if (_progressMode != ProgressMode::Subtasks)
        return;

    if (subtaskIndex >= _subtasks.count())
        return;

    _subtasks.setBit(subtaskIndex, true);

    updateProgress();

    const auto subtaskName = _subtasksNames[subtaskIndex];

    if (!progressDescription.isEmpty()) {
        privateSetProgressDescription(progressDescription);
    }
    else {
        if (!subtaskName.isEmpty())
            privateSetProgressDescription(subtaskName);
    }

    emit subtaskFinished(subtaskName);
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

    updateProgressText();
}

void Task::privateSetProgressTextFormatter(const ProgressTextFormatter& progressTextFormatter)
{
    _progressTextFormatter = progressTextFormatter;

    updateProgressText();
}

}
