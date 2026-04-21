// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowResult.h"

namespace mv::util
{

WorkflowResult::WorkflowResult() = default;

WorkflowResult::WorkflowResult(const WorkflowExecutionContext& context):
	_context(context)
{
}

bool WorkflowResult::isValid() const
{
	return _context.isValid();
}

bool WorkflowResult::hasErrors() const
{
	const auto reportNode = _context.getReportNode();
	return reportNode ? reportNode->hasErrorsRecursive() : false;
}

int WorkflowResult::getErrorCount() const
{
	const auto reportNode = _context.getReportNode();
	return reportNode ? reportNode->getErrorCountRecursive() : 0;
}

int WorkflowResult::getWarningCount() const
{
	const auto reportNode = _context.getReportNode();
	return reportNode ? reportNode->getWarningCountRecursive() : 0;
}

double WorkflowResult::getProgress() const
{
	return _context.getProgress();
}

const WorkflowExecutionContext& WorkflowResult::getContext() const
{
	return _context;
}

std::uint64_t WorkflowResult::getDuration() const
{
	return _duration;
}

void WorkflowResult::setDuration(std::uint64_t duration)
{
	_duration = duration;
}

QString WorkflowResult::getErrorMessage() const
{
	QStringList errorMessages;

	//if (const auto reportNode = _context.getReportNode()) {
	//	QVector<WorkflowMessage> messages;
	//	WorkflowReportNode::collectMessagesRecursive(reportNode, messages);
	//	for (const auto& message : messages)
	//	{
	//		if (message._level == WorkflowMessageLevel::Error)
	//			errorMessages.append(QString("[%1] %2").arg(message._source, message._text));
	//	}
	//}
	return errorMessages.join("\n");
}
}
