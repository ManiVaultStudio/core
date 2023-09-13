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
    { Task::Status::Aborted, "Aborted" }
});

QMap<Task::Scope, QString> Task::scopeNames = QMap<Scope, QString>({
    { Task::Scope::Background, "Background" },
    { Task::Scope::ForeGround, "ForeGround" },
    { Task::Scope::Modal, "Modal" }
});

Task::Task(QObject* parent, const QString& name, const Scope& scope /*= Scope::Background*/, const Status& status /*= Status::Undefined*/, bool mayKill /*= false*/, AbstractTaskHandler* handler /*= nullptr*/) :
    QObject(parent),
    Serializable(name),
    _name(name),
    _description(),
    _icon(),
    _status(status),
    _mayKill(mayKill),
    _handler(handler),
    _progressMode(ProgressMode::Manual),
    _scope(scope),
    _progress(0.f),
    _subtasks(),
    _subtasksNames(),
    _progressDescription(),
    _timers()
{
    if (core() != nullptr && core()->isInitialized())
        tasks().addTask(this);

    if (core() != nullptr && core()->isInitialized()) {
        QObject::connect(this, &QObject::destroyed, this, [this]() -> void {
            tasks().removeTask(this);
        });
    }

    for (auto& timer : _timers) {
        timer.setInterval(TASK_UPDATE_TIMER_INTERVAL);
        timer.setSingleShot(true);
    }

    connect(&getTimer(TimerType::ProgressChanged), &QTimer::timeout, this, [this]() -> void {
        emit progressChanged(_progress);

        QCoreApplication::processEvents();
    });

    connect(&getTimer(TimerType::ProgressDescriptionChanged), &QTimer::timeout, this, [this]() -> void {
        emit progressDescriptionChanged(_progressDescription);

        QCoreApplication::processEvents();
    });
}

Task::~Task()
{
}

Task* Task::getParentTask()
{
    return dynamic_cast<Task*>(parent());
}

bool Task::hasParentTask()
{
    return getParentTask() != nullptr;
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

QIcon Task::getIcon() const
{
    return _icon;
}

void Task::setIcon(const QIcon& icon)
{
    _icon = icon;

    emit iconChanged(_icon);
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
    if (status == _status)
        return;

    const auto previousStatus = _status;

    _status = status;

    updateProgress();

    emit statusChanged(previousStatus, _status);

    switch (_status)
    {
        case Task::Status::Idle:
        case Task::Status::Running:
        case Task::Status::RunningIndeterminate:
            break;

        case Task::Status::Finished:
            emit finished();
            break;

        case Task::Status::Aborting:
            break;

        case Task::Status::Aborted:
            emit aborted();
            break;

        default:
            break;
    }

    emit isKillableChanged(isKillable());

    QCoreApplication::processEvents();
}

void Task::setIdle()
{
    setStatus(Status::Idle);

    setProgressDescription("Idle", TASK_DESCRIPTION_DISAPPEAR_INTERVAL);
}

void Task::setRunning()
{
    setStatus(Status::Running);

    setProgressDescription("Running");
}

void Task::setRunningIndeterminate()
{
    setStatus(Status::RunningIndeterminate);

    setProgressDescription("Running");
}

void Task::setFinished(bool toIdleWithDelay /*= true*/, std::uint32_t delay /*= TASK_DESCRIPTION_DISAPPEAR_INTERVAL*/)
{
    setStatus(Status::Finished);

    setProgressDescription("Finished", TASK_DESCRIPTION_DISAPPEAR_INTERVAL);

    if (toIdleWithDelay) {
        QTimer::singleShot(delay, this, [this]() -> void {
            setIdle();
        });
    }
}

void Task::setFinished(const QString& progressDescription, bool toIdleWithDelay /*= true*/, std::uint32_t delay /*= TASK_DESCRIPTION_DISAPPEAR_INTERVAL*/)
{
    setStatus(Status::Finished);

    setProgressDescription(progressDescription, TASK_DESCRIPTION_DISAPPEAR_INTERVAL);

    if (toIdleWithDelay) {
        QTimer::singleShot(delay, this, [this]() -> void {
            setIdle();
        });
    }
}

void Task::setAborting()
{
    if (!getMayKill())
        return;

    setStatus(Status::Aborting);
    setProgressDescription("Aborting...");
}

void Task::setAborted()
{
    if (!getMayKill())
        return;

    setStatus(Status::Aborted);
    setProgress(0.f);

    emit aborted();

    setProgressDescription("Aborted", TASK_DESCRIPTION_DISAPPEAR_INTERVAL);
}

void Task::kill()
{
    if (getMayKill()) {
        setAborting();

        emit abort();
    }
    else {
        auto messageBox = new QMessageBox(nullptr);

        messageBox->setAttribute(Qt::WA_DeleteOnClose);
        messageBox->setWindowTitle("Unable to kill task");
        messageBox->setText(QString("<p><b>%1</b> cannot be killed because the <i>Task::aborted()</i> signal is not handled. See <a href='%2'>Task.h</a> and <a href='%3'>Task.cpp</a> for more details.</p>").arg(getName(), "https://github.com/ManiVaultStudio/core/tree/master/HDPS/src/Task.h", "https://github.com/ManiVaultStudio/core/tree/master/HDPS/src/Task.cpp"));
        messageBox->setWindowIcon(Application::getIconFont("FontAwesome").getIcon("exclamation-circle"));
        messageBox->setAttribute(Qt::WA_DeleteOnClose);

        messageBox->open();
    }
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

Task::Scope Task::getScope() const
{
    return _scope;
}

void Task::setScope(const Scope& scope)
{
    if (scope == _scope)
        return;

    _scope = scope;

    emit scopeChanged(_scope);
}

float Task::getProgress() const
{
    return _progress;
}

void Task::setProgress(float progress, const QString& subtaskDescription /*= ""*/)
{
    if (_progressMode != ProgressMode::Manual)
        return;

    progress = std::clamp(progress, 0.f, 1.f);

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
        {
            if (_progress == 0.f)
                return _name;
            else
                return QString("%1 %2%").arg(getProgressDescription().isEmpty() ? "" : QString("%1: ").arg(getProgressDescription()), QString::number(getProgress() * 100.f, 'f', 1));
        }

        case Task::Status::RunningIndeterminate:
            return getProgressDescription();

        case Task::Status::Finished:
            return QString("%1 finished").arg(getName());

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

    _subtasks.clear();

    _subtasks.resize(numberOfSubtasks);
    _subtasksNames.resize(numberOfSubtasks);
    
    emit subtasksChanged(_subtasks, _subtasksNames);

    updateProgress();
}

void Task::setSubtasks(const QStringList& subtasksNames)
{
    setProgressMode(ProgressMode::Subtasks);

    if (_subtasks.count() != subtasksNames.count())
        _subtasks.resize(subtasksNames.count() * 8);

    if (subtasksNames == _subtasksNames)
        return;

    _subtasksNames = subtasksNames;

    emit subtasksChanged(_subtasks, _subtasksNames);

    updateProgress();
}

void Task::addSubtasks(const QStringList& subtasksNames)
{
    if (subtasksNames.isEmpty())
        return;

    if (_progressMode != ProgressMode::Subtasks)
        return;

    _subtasksNames << subtasksNames;
    
    _subtasks.resize(_subtasksNames.count());

    emit subtasksAdded(subtasksNames);
    emit subtasksChanged(_subtasks, _subtasksNames);

    updateProgress();
}

void Task::setSubtaskFinished(std::uint32_t subtaskIndex, const QString& progressDescription /*= QString()*/)
{
    if (_progressMode != ProgressMode::Subtasks)
        return;

    if (subtaskIndex >= _subtasks.count())
        return;

    _subtasks.setBit(subtaskIndex, true);

    updateProgress();

    qDebug() << subtaskIndex << _progress;
    //emit subtasksChanged(_subtasks, _subtasksNames);

    const auto subtaskName = _subtasksNames[subtaskIndex];

    if (getProgress() < 1.f) {
        if (!progressDescription.isEmpty()) {
            setProgressDescription(progressDescription);
        }
        else {
            if (!subtaskName.isEmpty())
                setProgressDescription(subtaskName);
        }
    }

    emit subtaskFinished(subtaskName);

    QCoreApplication::processEvents();
}

void Task::setSubtaskFinished(const QString& subtaskName, const QString& progressDescription /*= QString()*/)
{
    if (_progressMode != ProgressMode::Subtasks)
        return;

    const auto subtaskIndex = getSubtaskIndex(subtaskName);

    if (subtaskIndex < 0)
        return;

    setSubtaskFinished(subtaskIndex, progressDescription);
}

void Task::setSubtaskStarted(const QString& subtaskName, const QString& progressDescription /*= QString()*/)
{
    if (_progressMode != ProgressMode::Subtasks)
        return;

    const auto subtaskIndex = getSubtaskIndex(subtaskName);

    if (subtaskIndex < 0)
        return;

    const auto verifiedSubtaskName = _subtasksNames[subtaskIndex];

    setProgressDescription(progressDescription.isEmpty() ? verifiedSubtaskName : progressDescription);

    emit subtaskStarted(verifiedSubtaskName);

    QCoreApplication::processEvents();
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

void Task::setProgressDescription(const QString& progressDescription, std::uint32_t clearDelay /*= 0*/)
{
    if (progressDescription == _progressDescription)
        return;

    _progressDescription = progressDescription;

    if (!getTimer(TimerType::ProgressDescriptionChanged).isActive())
        getTimer(TimerType::ProgressDescriptionChanged).start();

    if (clearDelay > 0) {
        QTimer::singleShot(clearDelay, this, [this]() -> void {
            setProgressDescription("");
        });
    }
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
    //qDebug() << (_subtasks.size() / 8) << _subtasks.count(true);

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

            auto accumulatedProgress = std::accumulate(childTasks.begin(), childTasks.end(), 0.f, [](float sum, Task* childTask) -> float {
                return sum + childTask->getProgress();
            });

            _progress = accumulatedProgress / static_cast<float>(childTasks.count());

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

    if (!getTimer(TimerType::ProgressChanged).isActive())
        getTimer(TimerType::ProgressChanged).start();

    if (_status == Status::Finished || _status == Status::Aborted)
        setProgressDescription("", TASK_DESCRIPTION_DISAPPEAR_INTERVAL);
}

QTimer& Task::getTimer(const TimerType& timerType)
{
    return _timers[static_cast<int>(timerType)];
}

void Task::childEvent(QChildEvent* event)
{
    const auto numberOfChildTasks = getChildTasks().count();

    switch (event->type())
    {
        case QEvent::ChildAdded:
        {
            auto childTask = dynamic_cast<Task*>(event->child());

            if (childTask == nullptr)
                break;

#ifdef TASK_VERBOSE
            qDebug() << "Child task was added";
#endif

            setProgressMode(ProgressMode::Aggregate);
            registerChildTask(childTask);
            updateStatus();
            updateProgress();

            break;
        }

        case QEvent::ChildRemoved:
        {
            auto childTask = dynamic_cast<Task*>(event->child());

            if (childTask == nullptr)
                break;

#ifdef TASK_VERBOSE
            qDebug() << "Child task was removed";
#endif

            unregisterChildTask(childTask);
            updateStatus();
            updateProgress();

            break;
        }

        default:
            break;
    }

    return QObject::childEvent(event);
}

Task::TasksPtrs Task::getChildTasks() const
{
    Task::TasksPtrs childTasksPtrs;

    for (auto childObject : children()) {
        auto childTask = dynamic_cast<Task*>(childObject);

        if (childTask == nullptr)
            continue;

        childTasksPtrs << childTask;
    }

    return childTasksPtrs;
}

void Task::registerChildTask(Task* childTask)
{
    Q_ASSERT(childTask != nullptr);

    if (childTask == nullptr)
        return;

    connect(childTask, &Task::statusChanged, this, [this](const Status& previousStatus, const Status& status) -> void {
        if (getProgressMode() != ProgressMode::Aggregate)
            return;

        updateStatus();
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
        
        setProgressDescription(progressDescription);
    });
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

void Task::updateStatus()
{
    if (_progressMode != ProgressMode::Aggregate)
        return;

    const auto childTasks = getChildTasks();

    const auto countStatus = [&childTasks](const Status& status) -> std::size_t {
        return std::accumulate(childTasks.begin(), childTasks.end(), 0.f, [status](std::size_t count, Task* childTask) -> std::size_t {
            return count + (childTask->getStatus() == status ? 1 : 0);
        });
    };

    if (countStatus(Status::Running) >= 1 || countStatus(Status::RunningIndeterminate) >= 1)
        setRunning();

    if (countStatus(Status::Idle) == childTasks.count())
        setIdle();

    if (countStatus(Status::Finished) == childTasks.count())
        setFinished(false);
}

}
