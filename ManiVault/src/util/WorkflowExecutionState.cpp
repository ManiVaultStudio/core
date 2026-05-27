// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowExecutionState.h"

#include <QMutexLocker>

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

void WorkflowExecutionState::setResultValue(const QString& key, const QVariant& value)
{
    if (key.trimmed().isEmpty())
        throw std::invalid_argument("Workflow result value key may not be empty");

    QMutexLocker lock(&_resultValuesMutex);

    _resultValues.insert(key, value);
}

QVariant WorkflowExecutionState::getResultValue(const QString& key) const
{
	QMutexLocker lock(&_resultValuesMutex);

    auto it = _resultValues.find(key);

    if (it == _resultValues.end())
        return {};

    return it.value();
}

bool WorkflowExecutionState::hasResultValue(const QString& key) const
{
    QMutexLocker lock(&_resultValuesMutex);

    return _resultValues.contains(key);
}

QVariant WorkflowExecutionState::takeResultValue(const QString& key)
{
    QMutexLocker lock(&_resultValuesMutex);

    auto it = _resultValues.find(key);

    if (it == _resultValues.end())
        return {};

    QVariant value = it.value();

    _resultValues.erase(it);

    return value;
}

QVariantMap WorkflowExecutionState::getResultValues(const QString& scope)
{
    QMutexLocker lock(&_resultValuesMutex);

    QVariantMap result;

    const auto prefix = scope.isEmpty() ? QString() : scope + "/";

    for (auto it = _resultValues.begin(); it != _resultValues.end(); ++it) {
        if (scope.isEmpty()) {
            result[it.key()] = it.value();
        } else if (it.key().startsWith(prefix)) {
            const auto localKey = it.key().mid(prefix.size());
            result[localKey] = it.value();
        }
    }

    return result;
}

QVariantMap WorkflowExecutionState::takeResultValues(const QString& scope)
{
    QMutexLocker lock(&_resultValuesMutex);

    QVariantMap result;

    const auto prefix = scope.endsWith("/") ? scope : scope + "/";

    for (auto it = _resultValues.begin(); it != _resultValues.end(); ) {
        if (it.key().startsWith(prefix)) {
            const auto localKey = it.key().mid(prefix.size());
            result[localKey] = it.value();
            it = _resultValues.erase(it);
        } else {
            ++it;
        }
    }

    return result;
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
