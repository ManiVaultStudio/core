// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowReportNode.h"

namespace mv::util
{

WorkflowReportNode::WorkflowReportNode(const QString& name):
	_name(name)
{
}

WorkflowReportNode::Ptr WorkflowReportNode::createChild(const QString& name)
{
	QMutexLocker lock(&_mutex);

	auto child = std::make_shared<WorkflowReportNode>(name);
	_children.push_back(child);
	return child;
}

void WorkflowReportNode::addMessage(SeverityLevel level, const QString& source, const QString& text, const QString& scope, const QVariantMap& details)
{
	QMutexLocker lock(&_mutex);

	_messages.push_back(WorkflowMessage{
		level,
		source,
		text,
		details,
		_name,
		QDateTime::currentDateTime()
	});
}

QString WorkflowReportNode::getName() const
{
	QMutexLocker lock(&_mutex);
	return _name;
}

QVector<WorkflowMessage> WorkflowReportNode::getMessages() const
{
	QMutexLocker lock(&_mutex);
	return _messages;
}

QVector<WorkflowReportNode::Ptr> WorkflowReportNode::getChildren() const
{
	QMutexLocker lock(&_mutex);
	return _children;
}

bool WorkflowReportNode::hasErrorsRecursive() const
{
	QVector<Ptr> childrenCopy;

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

int WorkflowReportNode::getWarningCountRecursive() const
{
	int          result = 0;
	QVector<Ptr> childrenCopy;

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

int WorkflowReportNode::getErrorCountRecursive() const
{
	int          result = 0;
	QVector<Ptr> childrenCopy;

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
