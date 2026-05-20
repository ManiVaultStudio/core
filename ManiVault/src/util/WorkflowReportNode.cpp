// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowReportNode.h"

namespace mv::util
{

WorkflowReportNode::WorkflowReportNode(QString name):
	_name(std::move(name))
{
}

WorkflowReportNode::SharedWorkflowReportNode WorkflowReportNode::createChild(const QString& name)
{
	QMutexLocker lock(&_mutex);

	auto child = std::make_shared<WorkflowReportNode>(name);

	_children.push_back(child);

	return child;
}

void WorkflowReportNode::addMessage(SeverityLevel level, QString emitter, QString text, QString location, QVariantMap details)
{
    qDebug()
        << "addMessage thread =" << QThread::currentThread()
        << "this =" << this
        << "messages size =" << _messages.size();

	QMutexLocker lock(&_mutex);

    auto context = WorkflowExecutionContext::current();

    thread_local bool inAddMessage = false;

    if (inAddMessage) {
        qFatal("Recursive addMessage detected");
    }

    inAddMessage = true;

    _messages.push_back(WorkflowMessage{
        level,
        context->getExecutionPath(),
        std::move(location),
        std::move(text),
        std::move(details),
        QDateTime::currentDateTimeUtc()
    });

    inAddMessage = false;
}

QString WorkflowReportNode::getName() const
{
	QMutexLocker lock(&_mutex);
	return _name;
}

WorkflowMessages WorkflowReportNode::getMessages() const
{
	QMutexLocker lock(&_mutex);
	return _messages;
}

WorkflowReportNode::SharedWorkflowReportNodes WorkflowReportNode::getChildren() const
{
	QMutexLocker lock(&_mutex);
	return _children;
}

bool WorkflowReportNode::hasErrorsRecursive() const
{
	SharedWorkflowReportNodes childrenCopy;

	{
		QMutexLocker lock(&_mutex);

		for (const auto& message : _messages) {
			if (message._level == SeverityLevel::Error)
				return true;
		}

		childrenCopy = _children;
	}

	for (const auto& child : childrenCopy) {
		if (child->hasErrorsRecursive())
			return true;
	}

	return false;
}

std::int32_t WorkflowReportNode::getWarningCountRecursive() const
{
    std::int32_t result = 0;

	SharedWorkflowReportNodes childrenCopy;

	{
		QMutexLocker lock(&_mutex);

		for (const auto& message : _messages) {
			if (message._level == SeverityLevel::Warning)
				++result;
		}

		childrenCopy = _children;
	}

	for (const auto& child : childrenCopy)
		result += child->getWarningCountRecursive();

	return result;
}

std::int32_t WorkflowReportNode::getErrorCountRecursive() const
{
    std::int32_t result = 0;

	SharedWorkflowReportNodes childrenCopy;

	{
		QMutexLocker lock(&_mutex);

		for (const auto& message : _messages) {
			if (message._level == SeverityLevel::Error)
				++result;
		}

		childrenCopy = _children;
	}

	for (const auto& child : childrenCopy)
		result += child->getErrorCountRecursive();

	return result;
}

}
