// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Task.h"
#include "CoreInterface.h"

namespace hdps {

using namespace util;

Task::Task(QObject* parent, const QString& name, const Status& status /*= Status::Idle*/, AbstractTaskHandler* handler /*= nullptr*/) :
    QObject(parent),
    Serializable(name),
    _name(name),
    _description(),
    _status(status),
    _handler(handler),
    _progress(0.f),
    _items()
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
}

AbstractTaskHandler* Task::getHandler()
{
    return _handler;
}

float Task::getProgress() const
{
    return _progress;
}

void Task::setProgress(float progress)
{
    if (progress == _progress)
        return;

    _progress = progress;

    emit progressChanged(_progress);
}

void Task::setNumberOfItems(std::uint32_t numberOfItems)
{
    if (numberOfItems == _items.count())
        return;

    _items.resize(numberOfItems);

    emit itemsChanged(_items);
}

void Task::setItemFinished(std::uint32_t itemIndex)
{
    if (itemIndex >= _items.count())
        return;

    _items.setBit(itemIndex, true);

    emit itemsChanged(_items);
}

void Task::start(std::uint32_t numberOfItems)
{
    setNumberOfItems(numberOfItems);
    setStatus(Status::Running);
}

}
