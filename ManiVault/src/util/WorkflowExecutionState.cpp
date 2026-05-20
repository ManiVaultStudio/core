// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowExecutionState.h"

namespace mv::util
{
WorkflowExecutionState::WorkflowExecutionState(const WorkflowReportNode::SharedWorkflowReportNode& reportRoot, const WorkflowProgressNode::Ptr& progressRoot, WorkflowExecutionOptions executionOptions /*= {}*/) :
	_reportRoot(reportRoot),
	_progressRoot(progressRoot),
    _executionOptions(executionOptions)
{
}

WorkflowReportNode::SharedWorkflowReportNode WorkflowExecutionState::getReportRoot() const
{
	return _reportRoot;
}

WorkflowProgressNode::Ptr WorkflowExecutionState::getProgressRoot() const
{
	return _progressRoot;
}

WorkflowExecutionOptions WorkflowExecutionState::getExecutionOptions() const
{
    return _executionOptions;
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
}

double WorkflowExecutionState::getOverallProgress() const
{
	return _progressRoot ? _progressRoot->getProgress() : 0.0;
}

WorkflowMessages WorkflowExecutionState::collectMessages() const
{
	QVector<WorkflowMessage> result;
	collectMessagesRecursive(_reportRoot, result);
	return result;
}

WorkflowExecutionMetrics& WorkflowExecutionState::metrics()
{
	return _metrics;
}

const WorkflowExecutionMetrics& WorkflowExecutionState::metrics() const
{
	return _metrics;
}

std::shared_ptr<AbstractWorkflowTraceSink> WorkflowExecutionState::getTraceSink() const
{
	return _executionOptions._traceSink;
}

void WorkflowExecutionState::trace(WorkflowTraceEvent event) const
{
	if (!getTraceSink())
		return;

	event._threadId    = QThread::currentThreadId();
	event._timestampNs = AbstractWorkflowTraceSink::currentTimestampNs();

    getTraceSink()->trace(event);
}

void WorkflowExecutionState::collectMessagesRecursive(const WorkflowReportNode::SharedWorkflowReportNode& node, QVector<WorkflowMessage>& out)
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
