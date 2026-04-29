// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowResult.h"

#include <optional>

namespace mv::util
{

WorkflowResult::WorkflowResult() = default;

WorkflowResult::WorkflowResult(WorkflowExecutionContext* context):
	_context(context)
{
}

bool WorkflowResult::isValid() const
{
	return _context ? _context->isValid() : false;
}

bool WorkflowResult::hasErrors() const
{
	const auto reportNode = _context ? _context->getReportNode() : nullptr;
	return reportNode ? reportNode->hasErrorsRecursive() : false;
}

int WorkflowResult::getErrorCount() const
{
	const auto reportNode = _context ? _context->getReportNode() : nullptr;
	return reportNode ? reportNode->getErrorCountRecursive() : 0;
}

int WorkflowResult::getWarningCount() const
{
	const auto reportNode = _context ? _context->getReportNode() : nullptr;
	return reportNode ? reportNode->getWarningCountRecursive() : 0;
}

double WorkflowResult::getProgress() const
{
	return _context ? _context->getProgress() : 0.0;
}

WorkflowExecutionContext* WorkflowResult::getContext() const
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

void WorkflowResult::setMetrics(QVector<WorkflowMetric> metrics)
{
    _metrics = std::move(metrics);
}

QVector<WorkflowMetric> WorkflowResult::getMetrics() const
{
    return _metrics;
}

std::optional<WorkflowMetric> WorkflowResult::getMetric(const QString& name) const
{
    for (const auto& metric : _metrics) {
        if (metric._name == name)
            return metric;
    }

    return std::nullopt;
}

}
