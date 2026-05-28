// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowProgressNode.h"

namespace mv::util
{

WorkflowProgressNode::WorkflowProgressNode(Type type, QString name, Ptr parent, double weight /*= 1.0*/) :
    _type(type),
    _name(std::move(name)),
    _parent(std::move(parent)),
    _weight(weight)
{
}

WorkflowProgressNode::Ptr WorkflowProgressNode::createRoot(Type type, const QString& name, double weight)
{
    return std::make_shared<WorkflowProgressNode>(type, name, nullptr, weight);
}

WorkflowProgressNode::Ptr WorkflowProgressNode::createChild(Type type, QString name, double weight /*= 1.0*/)
{
    auto child = std::make_shared<WorkflowProgressNode>(type, std::move(name), shared_from_this(), weight);

    {
        QMutexLocker lock(&_mutex);
        _children.push_back(child);
    }

    return child;
}

bool WorkflowProgressNode::hasChildren() const
{
	QMutexLocker lock(&_mutex);
	return !_children.isEmpty();
}

void WorkflowProgressNode::setProgress(double value)
{
	value = std::clamp(value, 0.0, 1.0);

	QMutexLocker lock(&_mutex);

	if (!_children.isEmpty()) {
		qWarning() << "setProgress() called on non-leaf progress node";
		return;
	}

	_selfProgress = value;
}

double WorkflowProgressNode::getProgress() const
{
    QVector<Ptr> childrenCopy;

    {
        QMutexLocker lock(&_mutex);

        if (_children.isEmpty())
            return _selfProgress;

        childrenCopy = _children;
    }

    double weightSum = 0.0;
    double weightedProgress = 0.0;

    for (const auto& child : childrenCopy) {
        if (!child)
            continue;

        const auto weight = child->getWeight();

        weightSum += weight;
        weightedProgress += child->getProgress() * weight;
    }

    if (weightSum <= 0.0)
        return 0.0;

    return weightedProgress / weightSum;
}

double WorkflowProgressNode::getWeight() const
{
	QMutexLocker lock(&_mutex);

	return _weight;
}

WorkflowProgressNode::Type WorkflowProgressNode::getType() const
{
    QMutexLocker locker(&_mutex);
    return _type;
}

QString WorkflowProgressNode::getName() const
{
    QMutexLocker locker(&_mutex);
    return _name;
}

WorkflowProgressNode::Status WorkflowProgressNode::getStatus() const
{
    QMutexLocker locker(&_mutex);
    return _status;
}

WorkflowProgressNode::Ptr WorkflowProgressNode::getParent() const
{
    QMutexLocker locker(&_mutex);
    return _parent;
}

QVector<WorkflowProgressNode::Ptr> WorkflowProgressNode::getChildren() const
{
    QMutexLocker locker(&_mutex);
    return _children;
}

int WorkflowProgressNode::getChildCount() const
{
    QMutexLocker locker(&_mutex);
    return _children.size();
}

int WorkflowProgressNode::getCompletedChildCount() const
{
    QVector<Ptr> childrenCopy;

    {
        QMutexLocker lock(&_mutex);
        childrenCopy = _children;
    }

    int count = 0;

    for (const auto& child : childrenCopy) {
        if (child && child->isFinished())
            ++count;
    }

    return count;
}

qint64 WorkflowProgressNode::getElapsedMilliseconds() const
{
    QMutexLocker locker(&_mutex);

    if (_status == Status::Pending)
        return 0;

    if (isFinishedUnlocked())
        return _finishedElapsedMs;

    return _timer.isValid() ? _timer.elapsed() : 0;
}

void WorkflowProgressNode::markRunning()
{
    setStatus(Status::Running);
}

void WorkflowProgressNode::markCompleted()
{
    setStatus(Status::Completed);
}

void WorkflowProgressNode::markFailed()
{
    setStatus(Status::Failed);
}

void WorkflowProgressNode::markSkipped()
{
    setStatus(Status::Skipped);
}

void WorkflowProgressNode::setStatus(Status status)
{
    QMutexLocker locker(&_mutex);

    if (_status == status)
        return;

    const auto wasPending = _status == Status::Pending;
    const auto wasRunning = _status == Status::Running;

    _status = status;

    if (status == Status::Running && wasPending)
        _timer.start();

    if ((status == Status::Completed || status == Status::Failed || status == Status::Skipped) && wasRunning)
        _finishedElapsedMs = _timer.isValid() ? _timer.elapsed() : 0;
}

bool WorkflowProgressNode::isFinished() const
{
    QMutexLocker lock(&_mutex);
    return isFinishedUnlocked();
}

bool WorkflowProgressNode::isFinishedUnlocked() const
{
    return _status == Status::Completed || _status == Status::Failed || _status == Status::Skipped;
}

}
