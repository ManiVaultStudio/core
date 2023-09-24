// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AbstractTaskHandler.h"
#include "Task.h"

namespace hdps {

AbstractTaskHandler::AbstractTaskHandler(QObject* parent, Task* task) :
    QObject(parent),
    _task(task),
    _minimumDuration(1500)
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
