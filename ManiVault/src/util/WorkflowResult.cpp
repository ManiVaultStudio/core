// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowResult.h"
#include "WorkflowExecutionState.h"

#include <optional>

namespace mv::util
{

bool WorkflowResult::hasErrors() const
{
    return false;
	//const auto reportNode = _context ? _context->getReportNode() : nullptr;
	//return reportNode ? reportNode->hasErrorsRecursive() : false;
}

int WorkflowResult::getErrorCount() const
{
    return {};
	//const auto reportNode = _context ? _context->getReportNode() : nullptr;
	//return reportNode ? reportNode->getErrorCountRecursive() : 0;
}

int WorkflowResult::getWarningCount() const
{
    return {};
	//const auto reportNode = _context ? _context->getReportNode() : nullptr;
	//return reportNode ? reportNode->getWarningCountRecursive() : 0;
}

double WorkflowResult::getProgress() const
{
    return {};
	//return _context ? _context->getProgress() : 0.0;
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

	for (const auto& message : getMessages())
	{
		//if (message._level == WorkflowMessageLevel::Error)
			errorMessages.append(QString("[%1] %2").arg(message._source, message._text));
	}
	
	return errorMessages.join("\n");
}

WorkflowMessages WorkflowResult::getMessages() const
{
    return _workflowMessages;
}

void WorkflowResult::setMessages(const WorkflowMessages& workflowMessages)
{
    _workflowMessages = workflowMessages;
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
