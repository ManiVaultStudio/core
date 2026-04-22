// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowExecutionState.h"

namespace mv::util
{
WorkflowExecutionState::WorkflowExecutionState(const WorkflowReportNode::Ptr& reportRoot, const WorkflowProgressNode::Ptr& progressRoot) :
	_reportRoot(reportRoot),
	_progressRoot(progressRoot),
	_notifier(new WorkflowExecutionNotifier())
{
}

WorkflowReportNode::Ptr WorkflowExecutionState::getReportRoot() const
{
	return _reportRoot;
}

WorkflowProgressNode::Ptr WorkflowExecutionState::getProgressRoot() const
{
	return _progressRoot;
}

WorkflowExecutionNotifier* WorkflowExecutionState::getNotifier() const
{
	return _notifier.get();
}

WorkflowExecutionStatus WorkflowExecutionState::getStatus() const
{
	QMutexLocker lock(&_mutex);
	return _status;
}

void WorkflowExecutionState::setStatus(WorkflowExecutionStatus status)
{
	{
		QMutexLocker lock(&_mutex);
		_status = status;
	}

	emit _notifier->statusChanged();
}

double WorkflowExecutionState::getOverallProgress() const
{
	return _progressRoot ? _progressRoot->getProgress() : 0.0;
}

void WorkflowExecutionState::notifyProgressChanged() const
{
	emit _notifier->progressChanged(getOverallProgress());
}

void WorkflowExecutionState::notifyMessagesChanged() const
{
	emit _notifier->messagesChanged();
}

QVector<WorkflowMessage> WorkflowExecutionState::collectMessages() const
{
	QVector<WorkflowMessage> result;
	collectMessagesRecursive(_reportRoot, result);
	return result;
}

void WorkflowExecutionState::collectMessagesRecursive(const WorkflowReportNode::Ptr& node, QVector<WorkflowMessage>& out)
{
	if (!node)
		return;

	const auto messages = node->getMessages();
	for (const auto& message : messages)
		out.push_back(message);

	const auto children = node->getChildren();
	for (const auto& child : children)
		collectMessagesRecursive(child, out);
}

}
