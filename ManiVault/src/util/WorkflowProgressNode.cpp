// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowProgressNode.h"

namespace mv::util
{

WorkflowProgressNode::WorkflowProgressNode(double weight): _weight(weight)
{
}

WorkflowProgressNode::Ptr WorkflowProgressNode::createChild(double weight)
{
	QMutexLocker lock(&_mutex);

	auto child = std::make_shared<WorkflowProgressNode>(weight);
	_children.push_back(child);
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
	double       selfProgress = 0.0;

	{
		QMutexLocker lock(&_mutex);

		if (_children.isEmpty())
			return _selfProgress;

		childrenCopy = _children;
		selfProgress = _selfProgress;
		Q_UNUSED(selfProgress);
	}

	double weightSum        = 0.0;
	double weightedProgress = 0.0;

	for (const auto& child : childrenCopy) {
		const double weight = child->getWeight();
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

}
