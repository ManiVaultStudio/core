// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowMetric.h"
#include "WorkflowMessage.h"

#include <QString>


namespace mv::util
{

class CORE_EXPORT WorkflowResult
{
public:

    WorkflowResult(const QString& workflowName);

public: // General

    QString getWorkflowName() const;

    std::uint64_t getDuration() const;

    void setDuration(std::uint64_t duration);

public: // Messages

    WorkflowMessages getMessages() const;
    WorkflowMessages getWarningMessages() const;
    WorkflowMessages getErrorMessages() const;
    WorkflowMessages getCriticalErrorMessages() const;
    void setMessages(const WorkflowMessages& workflowMessages);
    WorkflowMessages getMessagesByLevels(const WorkflowMessageLevels& workflowMessageLevels) const;
    int getMessageCountByLevels(const WorkflowMessageLevels& workflowMessageLevels) const;
    bool hasWarnings() const;
    bool hasErrors() const;
    bool hasCriticalErrors() const;
    int getWarningCount() const;
    int getErrorCount() const;
    int getCriticalErrorCount() const;

public: // Metrics

    void setMetrics(QVector<WorkflowMetric> metrics);

    QVector<WorkflowMetric> getMetrics() const;

    std::optional<WorkflowMetric> getMetric(const QString& name) const;

private:
    QString                     _workflowName;
    std::uint64_t               _duration = 0;
    bool                        _success = false;
    WorkflowMessages            _messages;
    QVector<WorkflowMetric>     _metrics;
};

using UniqueWorkflowResult = std::unique_ptr<WorkflowResult>;
using SharedWorkflowResult = std::shared_ptr<WorkflowResult>;

} // namespace mv::util