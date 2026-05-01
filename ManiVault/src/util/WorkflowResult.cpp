// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowResult.h"

#include <optional>
#include <ranges>

namespace mv::util
{

WorkflowResult::WorkflowResult(const QString& workflowName) :
    _workflowName(workflowName)
{
}

QString WorkflowResult::getWorkflowName() const
{
    return _workflowName;
}

std::uint64_t WorkflowResult::getDuration() const
{
    return _duration;
}

void WorkflowResult::setDuration(std::uint64_t duration)
{
    _duration = duration;
}

bool WorkflowResult::hasWarnings() const
{
    return getWarningCount() > 0;
}

bool WorkflowResult::hasErrors() const
{
    return getErrorCount() > 0;
}

bool WorkflowResult::hasCriticalErrors() const
{
    return getCriticalErrorCount() > 0;
}

int WorkflowResult::getWarningCount() const
{
    return getMessageCountByLevels({ WorkflowMessageLevel::Warning });
}

int WorkflowResult::getErrorCount() const
{
    return getMessageCountByLevels({ WorkflowMessageLevel::Error });
}

int WorkflowResult::getCriticalErrorCount() const
{
	return getMessageCountByLevels({ WorkflowMessageLevel::Critical });
}

WorkflowMessages WorkflowResult::getMessages() const
{
    return _messages;
}

WorkflowMessages WorkflowResult::getWarningMessages() const
{
    return getMessagesByLevels({ WorkflowMessageLevel::Warning });
}

WorkflowMessages WorkflowResult::getErrorMessages() const
{
	return getMessagesByLevels({ WorkflowMessageLevel::Error });
}

WorkflowMessages WorkflowResult::getCriticalErrorMessages() const
{
	return getMessagesByLevels({ WorkflowMessageLevel::Critical });
}

void WorkflowResult::setMessages(const WorkflowMessages& workflowMessages)
{
    _messages = workflowMessages;
}

WorkflowMessages WorkflowResult::getMessagesByLevels(const WorkflowMessageLevels& workflowMessageLevels) const
{
    auto filteredView = _messages | std::views::filter([workflowMessageLevels](const WorkflowMessage& message) {
        return workflowMessageLevels.contains(message._level);
    });

    return { filteredView.begin(), filteredView.end() };
}

int WorkflowResult::getMessageCountByLevels(const WorkflowMessageLevels& workflowMessageLevels) const
{
    return std::ranges::count_if(_messages, [workflowMessageLevels](const WorkflowMessage& message) {
        return workflowMessageLevels.contains(message._level);
    });
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
