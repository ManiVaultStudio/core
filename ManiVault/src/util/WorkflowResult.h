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

    bool isValid() const;

    bool hasErrors() const;

    int getErrorCount() const;

    int getWarningCount() const;

    double getProgress() const;

    std::uint64_t getDuration() const;

    void setDuration(std::uint64_t duration);

public: // Messages

    QString getErrorMessage() const;

    WorkflowMessages getMessages() const;
    void setMessages(const WorkflowMessages& workflowMessages);

public: // Metrics

    void setMetrics(QVector<WorkflowMetric> metrics);

    QVector<WorkflowMetric> getMetrics() const;

    std::optional<WorkflowMetric> getMetric(const QString& name) const;

private:
    std::uint64_t               _duration = 0;
    bool                        _success = false;
    WorkflowMessages            _workflowMessages;
    QVector<WorkflowMetric>     _metrics;
};

using UniqueWorkflowResult = std::unique_ptr<WorkflowResult>;

} // namespace mv::util