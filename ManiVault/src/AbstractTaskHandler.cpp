// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AbstractTaskHandler.h"
#include "Task.h"

namespace mv {

AbstractTaskHandler::AbstractTaskHandler(QObject* parent, Task* task) :
    QObject(parent),
    _task(task),
    _minimumDuration(100),
    _enabled(true)
{
}

Task* AbstractTaskHandler::getTask()
{
    return _task;
}

std::uint32_t AbstractTaskHandler::getMinimumDuration() const
{
    return _minimumDuration;
}

void AbstractTaskHandler::setMinimumDuration(std::uint32_t minimumDuration)
{
    if (minimumDuration == _minimumDuration)
        return;

    const auto previousMinimumDuration = _minimumDuration;

    _minimumDuration = minimumDuration;

    emit minimumDurationChanged(previousMinimumDuration, _minimumDuration);
}

bool AbstractTaskHandler::getEnabled() const
{
    return _enabled;
}

void AbstractTaskHandler::setEnabled(bool enabled)
{
    if (enabled == _enabled)
        return;

    _enabled = enabled;

    emit enabledChanged(_enabled);
}

void AbstractTaskHandler::setTask(Task* task)
{
    Q_ASSERT(task != nullptr);

    if (task == nullptr)
        return;

    const auto previousTask = _task;

    _task = task;

    emit taskChanged(previousTask, _task);
}

}
