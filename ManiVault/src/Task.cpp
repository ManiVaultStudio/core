// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Task.h"
#include "CoreInterface.h"

#ifdef _DEBUG
    //#define TASK_VERBOSE
#endif

namespace mv {

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
    { Task::GuiScope::Modal, "Modal" },
    { Task::GuiScope::DataHierarchy, "DataHierarchy" }
});

Task::Task(QObject* parent, const QString& name, const GuiScopes& guiScopes /*= { GuiScope::None }*/, const Status& status /*= Status::Undefined*/, bool mayKill /*= false*/, AbstractTaskHandler* handler /*= nullptr*/) :
    QObject(parent),
    Serializable(name),
    _configuration(0),
    _weight(1.f),
    _name(name),
    _enabled(true),
    _visible(true),
    _status(status),
    _deferredStatus(Status::Undefined),
    _deferredStatusRecursive(false),
    _mayKill(mayKill),
    _alwaysProcessEvents(false),
    _handler(handler),
    _progressMode(ProgressMode::Manual),
    _guiScopes(guiScopes),
    _progress(0.f),
    _timers(),
    _subtaskNamePrefix("Subtask"),
    _parentTask(nullptr)
{
    privateAddToTaskManager();
    
    for (auto& timer : _timers)
        timer.setSingleShot(true);

    _timers[static_cast<int>(TimerType::EmitProgressChanged)].setInterval(EMIT_CHANGED_TIMER_INTERVAL);
    _timers[static_cast<int>(TimerType::EmitProgressDescriptionChanged)].setInterval(EMIT_CHANGED_TIMER_INTERVAL);
    _timers[static_cast<int>(TimerType::EmitProgressTextChanged)].setInterval(EMIT_CHANGED_TIMER_INTERVAL);
    _timers[static_cast<int>(TimerType::DeferredStatus)].setInterval(DEFERRED_TASK_STATUS_INTERVAL);

    connect(&getTimer(TimerType::EmitProgressChanged), &QTimer::timeout, this, [this]() -> void {
        emit progressChanged(getProgress());
    });

    connect(&getTimer(TimerType::EmitProgressDescriptionChanged), &QTimer::timeout, this, [this]() -> void {
        emit progressDescriptionChanged(getProgressDescription());
    });

    connect(&getTimer(TimerType::EmitProgressTextChanged), &QTimer::timeout, this, [this]() -> void {
        emit progressTextChanged(getProgressText());
    });

    connect(&getTimer(TimerType::DeferredStatus), &QTimer::timeout, this, [this]() -> void {
        setStatus(_deferredStatus, _deferredStatusRecursive);
    });
}

Task::~Task()
{
    for (auto& timer : _timers)
        timer.stop();

    if (core() && core()->isAboutToBeDestroyed())
        return;
        
    if (hasParentTask())
        getParentTask()->removeChildTask(this);

    for (auto childTask : _childTasks)
        childTask->setParentTask(nullptr);

    if (core() != nullptr && core()->isInitialized() && !core()->isAboutToBeDestroyed())
        tasks().removeTask(this);
}

QString Task::getTypeName(bool humanFriendly /*= true*/) const
{
    auto metaObjectPrt = metaObject();

    if (metaObjectPrt == nullptr)
        return {};

    auto typeString = QString(metaObjectPrt->className());

    if (humanFriendly)
        typeString.replace("mv::", "");

    return typeString;
}

std::int32_t Task::getConfiguration() const
{
    return _configuration;
}

bool Task::isConfigurationFlagSet(ConfigurationFlag configurationFlag) const
{
    return _configuration & static_cast<std::int32_t>(configurationFlag);
}

void Task::setConfigurationFlag(ConfigurationFlag configurationFlag, bool unset /*= false*/, bool recursive /*= false*/)
{
    const auto flagSet = isConfigurationFlagSet(configurationFlag);

    if (unset)
        _configuration = _configuration & ~static_cast<std::int32_t>(configurationFlag);
    else
        _configuration |= static_cast<std::int32_t>(configurationFlag);

    emit configurationChanged(_configuration);

    if (recursive) {
        for (auto childTask : getChildTasks())
            childTask->setConfigurationFlag(configurationFlag, unset, recursive);
    }

    if (flagSet != isConfigurationFlagSet(configurationFlag))
        emit configurationFlagToggled(configurationFlag, isConfigurationFlagSet(configurationFlag));
}

void Task::setConfiguration(std::int32_t configuration, bool recursive /*= false*/)
{
    _configuration = configuration;

    emit configurationChanged(_configuration);

    if (recursive) {
        for (auto childTask : getChildTasks())
            childTask->setConfiguration(configuration, recursive);
    }
}

float Task::getWeight() const
{
    return _weight;
}

void Task::setWeight(float weight)
{
    _weight = std::max(0.01f, weight);

    updateProgress();
}

Task* Task::getParentTask()
{
    return _parentTask;
}

void Task::setParentTask(Task* parentTask)
{
    if (QThread::currentThread() == thread()) {
        privateSetParentTask(parentTask);
        return;
    }

    QMetaObject::invokeMethod(this, [this, parentTask] {
            privateSetParentTask(parentTask);
        },
        Qt::QueuedConnection
    );
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

    auto iterator = std::remove_if(childTasks.begin(), childTasks.end(), [&statuses](Task* childTask) -> bool {
        return !statuses.contains(childTask->getStatus());
    });

    childTasks.erase(iterator, childTasks.end());

    return childTasks;
}

Task::TasksPtrs Task::getChildTasksForGuiScopes(bool recursively /*= false*/, bool enabledOnly /*= true*/, const GuiScopes& guiScopes /*= GuiScopes()*/) const
{
    auto childTasks = getChildTasks(recursively, enabledOnly);

    auto iterator = std::remove_if(childTasks.begin(), childTasks.end(), [this, &guiScopes](Task* task) -> bool {
        return !doGuiScopesOverlap(task->getGuiScopes(), _guiScopes);
    });

    childTasks.erase(iterator, childTasks.end());

    return childTasks;
}

Task::TasksPtrs Task::getChildTasksForGuiScopesAndStatuses(bool recursively /*= false*/, bool enabledOnly /*= true*/, const GuiScopes& guiScopes /*= GuiScopes()*/, const Statuses& statuses /*= Statuses()*/) const
{
    auto childTasks = getChildTasks(recursively, enabledOnly);

    auto iterator = std::remove_if(childTasks.begin(), childTasks.end(), [this, &guiScopes, &statuses](Task* task) -> bool {
        return !doGuiScopesOverlap(task->getGuiScopes(), _guiScopes) || !statuses.contains(task->getStatus());
    });

    childTasks.erase(iterator, childTasks.end());

    return childTasks;
}

void Task::addChildTask(Task* childTask)
{
    if (QThread::currentThread() == thread()) {
        privateAddChildTask(childTask);
        return;
    }

    QMetaObject::invokeMethod(this, [this, childTask] {
        privateAddChildTask(childTask);
        },
        Qt::QueuedConnection
    );
}

void Task::removeChildTask(Task* childTask)
{
    if (QThread::currentThread() == thread()) {
        privateRemoveChildTask(childTask);
        return;
    }

    QMetaObject::invokeMethod(this, [this, childTask] {
			privateRemoveChildTask(childTask);
        },
        Qt::QueuedConnection
    );
}

QString Task::getName() const
{
    return _name;
}

void Task::setName(const QString& name)
{
    if (QThread::currentThread() == thread()) {
        privateSetName(name);
        return;
    }

    QMetaObject::invokeMethod(this, [this, name] {
			privateSetName(name);
        },
        Qt::QueuedConnection
    );
}

QString Task::getDescription() const
{
    return _description;
}

void Task::setDescription(const QString& description)
{
    if (QThread::currentThread() == thread()) {
        privateSetDescription(description);
        return;
    }

    QMetaObject::invokeMethod(this, [this, description] {
			privateSetDescription(description);
        },
        Qt::QueuedConnection
    );
}

QIcon Task::getIcon() const
{
    return _icon;
}

void Task::setIcon(const QIcon& icon)
{
    if (QThread::currentThread() == thread()) {
        privateSetIcon(icon);
        return;
    }

    QMetaObject::invokeMethod(this, [this, icon] {
			privateSetIcon(icon);
        },
        Qt::QueuedConnection
    );
}

bool Task::getEnabled() const
{
    return _enabled;
}

void Task::setEnabled(bool enabled, bool recursive /*= false*/)
{
    if (QThread::currentThread() == thread()) {
        privateSetEnabled(enabled, recursive);
        return;
    }

    QMetaObject::invokeMethod(this, [this, enabled, recursive] {
			privateSetEnabled(enabled, recursive);
        },
        Qt::QueuedConnection
    );
}

bool Task::getVisible() const
{
    return _visible;
}

void Task::setVisible(bool visible)
{
    if (QThread::currentThread() == thread()) {
        privateSetVisible(visible);
        return;
    }

    QMetaObject::invokeMethod(this, [this, visible] {
	       privateSetVisible(visible);
        },
        Qt::QueuedConnection
    );
}

bool Task::getMayKill() const
{
    return _mayKill;
}

void Task::setMayKill(bool mayKill, bool recursive /*= false*/)
{
    if (QThread::currentThread() == thread()) {
        privateSetMayKill(mayKill, recursive);
        return;
    }

    QMetaObject::invokeMethod(this, [this, mayKill, recursive] {
			privateSetMayKill(mayKill, recursive);
        },
        Qt::QueuedConnection
    );
}

bool Task::isKillable() const
{
    if (!(isRunning() || isRunningIndeterminate()))
        return false;

    return getMayKill();
}

void Task::reset(bool recursive /*= false*/)
{
    if (QThread::currentThread() == thread()) {
        privateReset(recursive);
        return;
    }

    QMetaObject::invokeMethod(this, [this, recursive] {
			privateReset(recursive);
        },
        Qt::QueuedConnection
    );
}

void Task::setAlwaysProcessEvents(bool alwaysProcessEvents)
{
    _alwaysProcessEvents = alwaysProcessEvents;
}

bool Task::getAlwaysProcessEvents() const
{
    return _alwaysProcessEvents;
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

void Task::setStatus(const Status& status, bool recursive /*= false*/)
{
    if (QThread::currentThread() == thread()) {
        privateSetStatus(status, recursive);
        return;
    }

    QMetaObject::invokeMethod(this, [this, status, recursive] {
			privateSetStatus(status, recursive);
        },
        Qt::QueuedConnection
    );
}

void Task::setStatusDeferred(const Status& status, bool recursive /*= false*/, std::uint32_t delay /*= DEFERRED_TASK_STATUS_INTERVAL*/)
{
    if (QThread::currentThread() == thread()) {
        privateSetStatusDeferred(status, recursive, delay);
        return;
    }

    QMetaObject::invokeMethod(this, [this, status, recursive, delay] {
			privateSetStatusDeferred(status, recursive, delay);
        },
        Qt::QueuedConnection
    );
}

void Task::setUndefined()
{
    if (QThread::currentThread() == thread()) {
        privateSetUndefined();
        return;
    }

    QMetaObject::invokeMethod(this, [this] {
			privateSetUndefined();
        },
        Qt::QueuedConnection
    );
}

void Task::setIdle()
{
    if (QThread::currentThread() == thread()) {
        privateSetIdle();
        return;
    }

    QMetaObject::invokeMethod(this, [this] {
			privateSetIdle();
        },
        Qt::QueuedConnection
    );
}

void Task::setRunning()
{   
    if (QThread::currentThread() == thread()) {
        privateSetRunning();
        privateSetProgressDescription(getName());
        return;
    }

    QMetaObject::invokeMethod(this, [this] {
        privateSetRunning();
        privateSetProgressDescription(getName());
        }, Qt::QueuedConnection);
}

void Task::setRunningIndeterminate()
{
    if (QThread::currentThread() == thread()) {
        privateSetRunningIndeterminate();
        return;
    }

    QMetaObject::invokeMethod(this, [this] {
			privateSetRunningIndeterminate();
        },
        Qt::QueuedConnection
    );
}

void Task::setFinished()
{
    if (QThread::currentThread() == thread()) {
        privateSetFinished();
        return;
    }

    QMetaObject::invokeMethod(this, [this] {
			privateSetFinished();
        },
        Qt::QueuedConnection
    );
}

void Task::setAboutToBeAborted()
{
    if (QThread::currentThread() == thread()) {
        privateSetAboutToBeAborted();
        return;
    }

    QMetaObject::invokeMethod(this, [this] {
			privateSetAboutToBeAborted();
        },
        Qt::QueuedConnection
    );
}

void Task::setAborting()
{
    if (QThread::currentThread() == thread()) {
        privateSetAborting();
        return;
    }

    QMetaObject::invokeMethod(this, [this] {
			privateSetAborting();
        },
        Qt::QueuedConnection
    );
}

void Task::setAborted()
{
    if (QThread::currentThread() == thread()) {
        privateSetAborted();
        return;
    }

    QMetaObject::invokeMethod(this, [this] {
			privateSetAborted();
        },
        Qt::QueuedConnection
    );
}

void Task::kill(bool recursive /*= true*/)
{
    if (QThread::currentThread() == thread()) {
        privateKill();
        return;
    }

    QMetaObject::invokeMethod(this, [this] {
			privateKill();
        },
        Qt::QueuedConnection
    );
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
    if (QThread::currentThread() == thread()) {
        privateSetProgressMode(progressMode);
        return;
    }

    QMetaObject::invokeMethod(this, [this, progressMode] {
			privateSetProgressMode(progressMode);
        },
        Qt::QueuedConnection
    );
}

Task::GuiScopes Task::getGuiScopes() const
{
    return _guiScopes;
}

void Task::setGuiScopes(const GuiScopes& guiScopes)
{
    if (QThread::currentThread() == thread()) {
        privateSetGuiScopes(guiScopes);
        return;
    }

    QMetaObject::invokeMethod(this, [this, guiScopes] {
			privateSetGuiScopes(guiScopes);
		},
        Qt::QueuedConnection
    );
}

void Task::addGuiScope(const GuiScope& guiScope)
{
    if (QThread::currentThread() == thread()) {
        privateAddGuiScope(guiScope);
        return;
    }

    QMetaObject::invokeMethod(this, [this, guiScope] {
			privateAddGuiScope(guiScope);
        },
        Qt::QueuedConnection
    );
}

void Task::removeGuiScope(const GuiScope& guiScope)
{
    if (QThread::currentThread() == thread()) {
        privateRemoveGuiScope(guiScope);
        return;
    }

    QMetaObject::invokeMethod(this, [this, guiScope] {
			privateRemoveGuiScope(guiScope);
        },
        Qt::QueuedConnection
    );
}

bool Task::doGuiScopesOverlap(const GuiScopes& guiScopesA, const GuiScopes& guiScopesB) const
{
    for (const auto& guiScopeA : guiScopesA)
        if (guiScopesB.contains(guiScopeA))
            return true;

    return false;
}

QStringList Task::guiScopesToStringlist(const GuiScopes& guiScopes)
{
    QStringList guiScopesStringList;

    for (const auto& guiScope : guiScopes)
        guiScopesStringList << Task::guiScopeNames[guiScope];

    return guiScopesStringList;
}

float Task::getProgress() const
{
    return _progress;
}

void Task::setProgress(float progress, const QString& subtaskDescription /*= ""*/)
{
    if (QThread::currentThread() == thread()) {
        privateSetProgress(progress, subtaskDescription);
        return;
    }

    QMetaObject::invokeMethod(this, [this, progress, subtaskDescription] {
			privateSetProgress(progress, subtaskDescription);
        },
        Qt::QueuedConnection
    );
}

void Task::resetProgress(bool recursive /*= false*/)
{
    if (QThread::currentThread() == thread()) {
        privateResetProgress(recursive);
        return;
    }

    QMetaObject::invokeMethod(this, [this, recursive] {
			privateResetProgress(recursive);
        },
        Qt::QueuedConnection
    );
}

QTimer& Task::getTimer(const TimerType& timerType)
{
    return _timers[static_cast<int>(timerType)];
}

void Task::setTimerInterval(const TimerType& timerType, std::uint32_t interval)
{
    getTimer(timerType).setInterval(interval);
}

void Task::setSubtasks(std::uint32_t numberOfSubtasks)
{
    if (QThread::currentThread() == thread()) {
        privateSetSubtasks(numberOfSubtasks);
        return;
    }

    QMetaObject::invokeMethod(this, [this, numberOfSubtasks] {
			privateSetSubtasks(numberOfSubtasks);
        },
        Qt::QueuedConnection
    );
}

void Task::setSubtasks(const QStringList& subtasksNames)
{
    if (QThread::currentThread() == thread()) {
        privateSetSubtasks(subtasksNames);
        return;
    }

    QMetaObject::invokeMethod(this, [this, subtasksNames] {
			privateSetSubtasks(subtasksNames);
        },
        Qt::QueuedConnection
    );
}

void Task::setSubtaskStarted(std::uint32_t subtaskIndex, const QString& progressDescription /*= QString()*/)
{
    if (QThread::currentThread() == thread()) {
        privateSetSubtaskStarted(subtaskIndex, progressDescription);
        return;
    }

    QMetaObject::invokeMethod(this, [this, subtaskIndex, progressDescription] {
        privateSetSubtaskStarted(subtaskIndex, progressDescription);
        },
        Qt::QueuedConnection
    );
}

void Task::setSubtaskStarted(const QString& subtaskName, const QString& progressDescription /*= QString()*/)
{
    if (QThread::currentThread() == thread()) {
        privateSetSubtaskStarted(subtaskName, progressDescription);
        return;
    }

    QMetaObject::invokeMethod(this, [this, subtaskName, progressDescription] {
			privateSetSubtaskStarted(subtaskName, progressDescription);
        },
        Qt::QueuedConnection
    );
}

void Task::setSubtaskFinished(std::uint32_t subtaskIndex, const QString& progressDescription /*= QString()*/)
{
    if (QThread::currentThread() == thread()) {
        privateSetSubtaskFinished(subtaskIndex, progressDescription);
        return;
    }

    QMetaObject::invokeMethod(this, [this, subtaskIndex, progressDescription] {
			privateSetSubtaskFinished(subtaskIndex, progressDescription);
        },
        Qt::QueuedConnection
    );
}

void Task::setSubtaskFinished(const QString& subtaskName, const QString& progressDescription /*= QString()*/)
{
    if (QThread::currentThread() == thread()) {
        privateSetSubtaskFinished(subtaskName, progressDescription);
        return;
    }

    QMetaObject::invokeMethod(this, [this, subtaskName, progressDescription]() {
            privateSetSubtaskFinished(subtaskName, progressDescription);
        },
        Qt::QueuedConnection
    );
}

void Task::setSubtaskName(std::uint32_t subtaskIndex, const QString& subtaskName)
{
    if (QThread::currentThread() == thread()) {
        privateSetSubtaskName(subtaskIndex, subtaskName);
        return;
    }

    QMetaObject::invokeMethod(this, [this, subtaskIndex, subtaskName] {
			privateSetSubtaskName(subtaskIndex, subtaskName);
        },
        Qt::QueuedConnection
    );
}

QStringList Task::getSubtasksNames() const
{
    return _subtasksNames;
}

QString Task::getSubtasksName(std::uint32_t subtaskIndex) const
{
    if (subtaskIndex >= _subtasksNames.count())
        return QString("%1 %2").arg(getSubtaskNamePrefix(), QString::number(subtaskIndex));

    return _subtasksNames[subtaskIndex];
}

std::int32_t Task::getSubtaskIndex(const QString& subtaskName) const
{
    if (_progressMode != ProgressMode::Subtasks)
        return -1;

    if (!_subtasksNames.contains(subtaskName))
        return -1;

    return _subtasksNames.indexOf(subtaskName);
}

QString Task::getSubtaskNamePrefix() const
{
    return _subtaskNamePrefix;
}

void Task::setSubtaskNamePrefix(const QString& subtaskNamePrefix)
{
    if (subtaskNamePrefix == _subtaskNamePrefix)
        return;

    const auto previousSubtaskNamePrefix = _subtaskNamePrefix;

    _subtaskNamePrefix = subtaskNamePrefix;

    emit subtaskNamePrefixChanged(previousSubtaskNamePrefix, _subtaskNamePrefix);
}

QString Task::getProgressDescription() const
{
    return _progressDescription;
}

void Task::setProgressDescription(const QString& progressDescription, std::uint32_t clearDelay /*= 0*/)
{
    if (QThread::currentThread() == thread()) {
        privateSetProgressDescription(progressDescription, clearDelay);
        return;
    }

    QMetaObject::invokeMethod(this, [this, progressDescription, clearDelay] {
			privateSetProgressDescription(progressDescription, clearDelay);
        },
        Qt::QueuedConnection
    );
}

QString Task::getProgressText() const
{
    return _progressText;
}

void Task::setProgressTextFormatter(const ProgressTextFormatter& progressTextFormatter)
{
    if (QThread::currentThread() == thread()) {
        privateSetProgressTextFormatter(progressTextFormatter);
        return;
    }

    QMetaObject::invokeMethod(this, [this, progressTextFormatter] {
			privateSetProgressTextFormatter(progressTextFormatter);
        },
        Qt::QueuedConnection
    );
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
            return QString("%1 %2%").arg(getProgressDescription().isEmpty() ? "" : QString("%1").arg(getProgressDescription()), QString::number(getProgress() * 100.f, 'f', 1));

        case Task::Status::RunningIndeterminate:
            return QString("%1: %2").arg(getName(), getProgressDescription());

        case Task::Status::Finished:
            return "Finished";

        case Task::Status::AboutToBeAborted:
            return "About to be aborted";

        case Task::Status::Aborting:
            return "Aborting";

        case Task::Status::Aborted:
            return "Aborted";

        default:
            break;
    }

    return {};
}

void Task::updateProgress()
{
//#ifdef TASK_VERBOSE
//    qDebug() << __FUNCTION__ << getName();
//#endif

    if (!core())
        return;

    switch (_progressMode) {
        case ProgressMode::Manual:
            break;

        case ProgressMode::Subtasks:
        {
            _progress = _subtasks.isEmpty() ? 0.f : static_cast<float>(_subtasks.count(true)) / static_cast<float>(_subtasks.size());
            break;
        }

        case ProgressMode::Aggregate:
        {
            const auto childTasks = getChildTasksForStatuses(false, true, { Status::Undefined, Status::Idle, Status::Running, Status::RunningIndeterminate, Status::Finished });

            if (!childTasks.isEmpty()) {
                float interval = 0.f;

                auto accumulatedProgress = std::accumulate(childTasks.begin(), childTasks.end(), 0.f, [&interval](float sum, Task* childTask) -> float {
                    const auto& weight = childTask->getWeight();

                    interval += weight;

                    return sum + (weight * childTask->getProgress());
                });

                _progress = accumulatedProgress / interval;
            }

            break;
        }
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

    privateEmitProgressChanged();
    updateProgressText();
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

    privateEmitProgressTextChanged();
}

void Task::registerChildTask(Task* childTask)
{
    Q_ASSERT(childTask != nullptr);

    if (childTask == nullptr)
        return;

    connect(childTask, &Task::statusChanged, this, [this, childTask](const Status& previousStatus, const Status& status) -> void {
        if (getProgressMode() != ProgressMode::Aggregate)
            return;

        if (previousStatus == Status::Finished && status == Status::Idle)
            return;

        updateAggregateStatus();
        updateProgress();
    });

    connect(childTask, &Task::progressChanged, this, [this](float progress) -> void {
        if (getProgressMode() != ProgressMode::Aggregate)
            return;

        updateProgress();
        updateProgressText();
    });

    connect(childTask, &Task::progressDescriptionChanged, this, [this](const QString& progressDescription) -> void {
        if (getProgressMode() != ProgressMode::Aggregate)
            return;

        privateSetProgressDescription(progressDescription);
    });

    updateProgress();
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
    if (core() == nullptr)
        return;

    if (_progressMode != ProgressMode::Aggregate)
        return;

    if (isConfigurationFlagSet(ConfigurationFlag::OverrideAggregateStatus))
        return;

    const auto enabledChildTasks            = getChildTasks(false, true);
    const auto numberOfEnabledChildTasks    = enabledChildTasks.size();

    const auto getNumberOfChildTaskWithStatus = [this, &enabledChildTasks](const Status& status) -> std::size_t {
        return getChildTasksForStatuses(false, true, { status }).count();
    };
    
    if (getNumberOfChildTaskWithStatus(Status::Running) >= 1 || getNumberOfChildTaskWithStatus(Status::RunningIndeterminate) >= 1)
        privateSetRunning();

    if (getNumberOfChildTaskWithStatus(Status::Idle) == numberOfEnabledChildTasks)
        privateSetProgress(0.f);

    if (getNumberOfChildTaskWithStatus(Status::Aborted) == numberOfEnabledChildTasks)
        privateSetAborted();

    if (getNumberOfChildTaskWithStatus(Status::Finished) >= 1 && getNumberOfChildTaskWithStatus(Status::Finished) == numberOfEnabledChildTasks)
        privateSetFinished();

    updateProgress();
}

void Task::privateSetParentTask(Task* parentTask)
{
    try {
        const auto previousParentTask = _parentTask;

        if (parentTask == _parentTask)
            return;

        if (parentTask != nullptr) {
            if (getId() == parentTask->getId())
                return;
        }

        /*
        if (parentTask && (parentTask->getTypeName() != getTypeName()))
            qDebug() << QString("Type mismatch: child type is %1 and parent type is %2").arg(getTypeName(), parentTask->getTypeName());
        */

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

void Task::privateSetEnabled(bool enabled, bool recursive /*= false*/)
{
    if (enabled == _enabled)
        return;

    _enabled = enabled;

    if (recursive)
        for (auto childTask : getChildTasks(true, false))
            childTask->setEnabled(enabled, recursive);

    emit enabledChanged(_enabled);
}

void Task::privateSetVisible(bool visible)
{
    if (visible == _visible)
        return;

    _visible = visible;

    emit visibileChanged(_visible);
}

void Task::privateSetMayKill(bool mayKill, bool recursive /*= false*/)
{
    if (mayKill == _mayKill)
        return;

    _mayKill = mayKill;

    if (recursive)
        for (auto childTask : getChildTasks(true, false))
            childTask->setMayKill(mayKill, recursive);

    emit mayKillChanged(getMayKill());
    emit isKillableChanged(isKillable());
}

void Task::privateReset(bool recursive /*= false*/)
{
    _progress = 0.f;

    _subtasks.resize(0);
    _subtasksNames.resize(0);

    _progressDescription    = "";
    _progressText           = "";

    if (recursive)
        for (auto childTask : getChildTasks(true, false))
            childTask->reset(recursive);
}

void Task::privateSetStatus(const Status& status, bool recursive /*= false*/)
{
    if (recursive)
        for (auto childTask : getChildTasks(true, false))
            childTask->setStatus(status, recursive);

    if (status == _status)
        return;

    const auto previousStatus = _status;

    switch (status)
    {
        case Task::Status::Undefined:
        {
            _status = status;
    
            emit statusChangedToUndefined();
    
            break;
        }
    
        case Task::Status::Idle:
        {
            _status = status;
    
            emit statusChangedToIdle();
    
            break;
        }
    
        case Task::Status::Running:
        {
            _status = status;
    
            emit statusChangedToRunning();
    
            break;
        }
    
        case Task::Status::RunningIndeterminate:
        {
            _status = status;
            
            emit statusChangedToRunningIndeterminate();
    
            break;
        }
    
        case Task::Status::Finished:
        {
            _status = status;
            
            emit statusChangedToFinished();
    
            break;
        }
    
        case Task::Status::AboutToBeAborted:
        {
            if (!getMayKill())
                break;
    
            _status = status;
            
            emit statusChangedToAboutToBeAborted();
    
            break;
        }
    
        case Task::Status::Aborting:
        {
            if (!getMayKill())
                break;
    
            _status = status;
            
            emit statusChangedToAborting();
    
            break;
        }
    
        case Task::Status::Aborted:
        {
            if (!getMayKill())
                break;
    
            _status = status;
    
            emit statusChangedToAborted();
    
            break;
        }
    
        default:
            break;
    }

    updateProgress();

    emit statusChanged(previousStatus, _status);
    emit isKillableChanged(isKillable());
}

void Task::privateSetStatusDeferred(const Status& status, bool recursive /*= false*/, std::uint32_t delay /*= DEFERRED_TASK_STATUS_INTERVAL*/)
{
    _deferredStatus             = status;
    _deferredStatusRecursive    = recursive;

    getTimer(TimerType::DeferredStatus).start(delay);
}

void Task::privateSetUndefined()
{
    privateSetStatus(Status::Undefined);
}

void Task::privateSetIdle()
{
    privateSetStatus(Status::Idle);
}

void Task::privateSetRunning()
{
    privateSetStatus(Status::Running);
}

void Task::privateSetRunningIndeterminate()
{
    privateSetStatus(Status::RunningIndeterminate);
}

void Task::privateSetFinished()
{
    privateSetStatus(Status::Finished);

    if (!hasParentTask())
        privateSetStatusDeferred(Status::Idle, false, DEFERRED_TASK_STATUS_INTERVAL);

    //if (getProgressMode() == ProgressMode::Aggregate)
    //    for (auto childTask : getChildTasks(true, false))
    //        childTask->setIdle();
}

void Task::privateSetAboutToBeAborted()
{
    privateSetStatus(Status::AboutToBeAborted);
}

void Task::privateSetAborting()
{
    privateSetStatus(Status::Aborting);
}

void Task::privateSetAborted()
{
    privateSetStatus(Status::Aborted);
}

void Task::privateKill(bool recursive /*= true*/)
{
    if (recursive) {
        auto tasksToKill = getChildTasks();

        std::reverse(tasksToKill.begin(), tasksToKill.end());

        for (auto taskToKill : tasksToKill)
            taskToKill->privateKill();
    }

    if (getMayKill() && (isRunning() || isRunningIndeterminate())) {
        privateSetAboutToBeAborted();
        privateSetAborting();
        
        emit requestAbort();
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

void Task::privateSetGuiScopes(const GuiScopes& guiScopes)
{
    if (guiScopes == _guiScopes)
        return;

    _guiScopes = guiScopes;

    emit guiScopesChanged(_guiScopes);
}

void Task::privateAddGuiScope(const GuiScope& guiScope)
{
    if (_guiScopes.contains(guiScope))
        return;

    _guiScopes << guiScope;

    emit guiScopesChanged(_guiScopes);
}

void Task::privateRemoveGuiScope(const GuiScope& guiScope)
{
    if (!_guiScopes.contains(guiScope))
        return;

    _guiScopes.remove(guiScope);

    emit guiScopesChanged(_guiScopes);
}

void Task::privateResetProgress(bool recursive /*= false*/)
{
    _progress = 0.f;

    if (recursive)
        for (auto childTask : getChildTasks(recursive, false))
            childTask->privateResetProgress(recursive);
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

    privateEmitProgressChanged();

    updateProgressText();
}

void Task::privateSetSubtasks(std::uint32_t numberOfSubtasks)
{
    privateSetProgressMode(ProgressMode::Subtasks);

    if (numberOfSubtasks == 0)
        return;

    const auto numberOfBits = numberOfSubtasks;

    _subtasks.clear();
    _subtasks.resize(numberOfBits);
    _subtasks.fill(false, numberOfBits);

    //_subtasksNames.resize(numberOfSubtasks);

    emit subtasksChanged(_subtasks, _subtasksNames);

    updateProgress();
}

void Task::privateSetSubtasks(const QStringList& subtasksNames)
{
    privateSetProgressMode(ProgressMode::Subtasks);

    const auto numberOfBits = subtasksNames.count();

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

    const auto subtaskName = getSubtasksName(subtaskIndex);

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
        privateSetProgressDescription(subtaskName.isEmpty() ? getSubtasksName(subtaskIndex) : subtaskName);
    else
        privateSetProgressDescription(progressDescription);
}

void Task::privateSetSubtaskFinished(std::uint32_t subtaskIndex, const QString& progressDescription)
{
    if (_progressMode != ProgressMode::Subtasks) {
        qDebug() << "return: not in Subtasks mode";
        return;
    }

    if (subtaskIndex >= _subtasks.count()) {
        qDebug() << "return: index out of range";
        return;
    }

    _subtasks.setBit(subtaskIndex, true);
    updateProgress();

    const auto subtaskName = getSubtasksName(subtaskIndex);

    if (progressDescription.isEmpty())
        privateSetProgressDescription(subtaskName);
    else
        privateSetProgressDescription(progressDescription);

    emit subtaskFinished(subtaskIndex, subtaskName);
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

    if (subtaskIndex >= _subtasksNames.count())
        return;

    _subtasksNames[subtaskIndex] = subtaskName;
}

void Task::privateSetProgressDescription(const QString& progressDescription, std::uint32_t clearDelay /*= 0*/)
{
    if (progressDescription == _progressDescription)
        return;

    _progressDescription = progressDescription;

    privateEmitProgressDescriptionChanged();

    updateProgressText();
}

void Task::privateSetProgressText(const QString& progressText, std::uint32_t clearDelay /*= 0*/)
{
    if (progressText == _progressText)
        return;

    _progressText = progressText;

    privateEmitProgressTextChanged();
}

void Task::privateSetProgressTextFormatter(const ProgressTextFormatter& progressTextFormatter)
{
    _progressTextFormatter = progressTextFormatter;

    updateProgressText();
}

void Task::privateEmitProgressChanged()
{
    emit progressChanged(getProgress());
    QCoreApplication::processEvents();
    return;
    //qDebug() << __FUNCTION__ << getProgress();

    if (!getTimer(TimerType::EmitProgressChanged).isActive()) {
        emit progressChanged(getProgress());

        getTimer(TimerType::EmitProgressChanged).start();
    }

    if (_alwaysProcessEvents)
        QCoreApplication::processEvents();
}

void Task::privateEmitProgressDescriptionChanged()
{
    //qDebug() << __FUNCTION__ << getProgressDescription();

    if (!getTimer(TimerType::EmitProgressDescriptionChanged).isActive()) {
        emit progressDescriptionChanged(getProgressDescription());

        getTimer(TimerType::EmitProgressDescriptionChanged).start();
    }

    if (_alwaysProcessEvents)
        QCoreApplication::processEvents();
}

void Task::privateEmitProgressTextChanged()
{
    //qDebug() << __FUNCTION__ << getProgressText();

    if (!getTimer(TimerType::EmitProgressTextChanged).isActive()) {
        emit progressTextChanged(getProgressText());

        getTimer(TimerType::EmitProgressTextChanged).start();
    }

    if (_alwaysProcessEvents)
        QCoreApplication::processEvents();
}

void Task::addToTaskManager()
{
#ifdef _DEBUG
    qDebug() << "Manually adding task" << getName() << "to the task manager (this approach is off-label)";
#endif

    privateAddToTaskManager();
}

void Task::privateAddToTaskManager()
{
    if (core() != nullptr) {
        tasks().addTask(this);
    }
    else {
#ifdef _DEBUG
        qDebug() << "Cannot add task" << getName() << "to the task manager because the core has not been initialized yet";
#endif
    }
}

}
