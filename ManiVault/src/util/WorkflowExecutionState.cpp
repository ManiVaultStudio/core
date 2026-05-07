// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowExecutionState.h"

namespace mv::util
{
WorkflowExecutionState::WorkflowExecutionState(const WorkflowReportNode::Ptr& reportRoot, const WorkflowProgressNode::Ptr& progressRoot, WorkflowExecutionOptions executionOptions /*= {}*/) :
	_reportRoot(reportRoot),
	_progressRoot(progressRoot),
    _executionOptions(executionOptions)
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

void WorkflowExecutionState::setTraceSink(std::shared_ptr<AbstractWorkflowTraceSink> traceSink)
{
    Q_ASSERT(traceSink);

    if (_traceSink) {
        Q_ASSERT(_traceSink == traceSink);
        return;
    }

    _traceSink = std::move(traceSink);
}

std::shared_ptr<AbstractWorkflowTraceSink> WorkflowExecutionState::getTraceSink() const
{
	return _traceSink;
}

void WorkflowExecutionState::trace(WorkflowTraceEvent event)
{
	if (!_traceSink)
		return;

	event._threadId    = QThread::currentThreadId();
	event._timestampNs = AbstractWorkflowTraceSink::currentTimestampNs();

	_traceSink->trace(event);
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
