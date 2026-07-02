// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowResultBase.h"
#include "WorkflowMetric.h"
#include "WorkflowMessage.h"

#include <QString>

namespace mv::workflow
{

/**
 * @brief Stores the final outcome of a workflow execution.
 *
 * WorkflowResult extends WorkflowResultBase with workflow-level execution
 * details: total duration, emitted messages, severity-based message queries,
 * and collected metrics. The optional output value and execution status are
 * inherited from WorkflowResultBase.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowResult final : public WorkflowResultBase
{
public:

    /**
     * @brief Constructs a workflow result.
     * @param workflowName Name of the workflow that produced this result.
     */
    WorkflowResult(const QString& workflowName);

public: // General

    /**
     * @brief Returns the workflow execution duration.
     * @return Duration in milliseconds.
     */
    [[nodiscard]] std::uint64_t getDurationMS() const;

    /**
     * @brief Sets the workflow execution duration.
     * @param duration Duration in milliseconds.
     */
    void setDurationMS(std::uint64_t duration);

public: // Messages

    /**
     * @brief Returns all messages emitted during workflow execution.
     * @return Copy of the workflow message list.
     */
    [[nodiscard]] WorkflowMessages getMessages() const;

    /**
     * @brief Returns warning messages emitted during workflow execution.
     * @return Messages with Warning severity.
     */
    [[nodiscard]] WorkflowMessages getWarningMessages() const;

    /**
     * @brief Returns error messages emitted during workflow execution.
     * @return Messages with Error severity.
     */
    [[nodiscard]] WorkflowMessages getErrorMessages() const;

    /**
     * @brief Returns fatal error messages emitted during workflow execution.
     * @return Messages with Fatal severity.
     */
    [[nodiscard]] WorkflowMessages getFatalErrorMessages() const;

    /**
     * @brief Replaces the workflow message list.
     * @param workflowMessages Messages to store on this result.
     */
    void setMessages(const WorkflowMessages& workflowMessages);

    /**
     * @brief Returns messages whose severity is included in the requested set.
     * @param severityLevels Severity levels to include.
     * @return Filtered workflow messages.
     */
    [[nodiscard]] WorkflowMessages getMessagesByLevels(const util::SeverityLevels& severityLevels) const;

    /**
     * @brief Counts messages whose severity is included in the requested set.
     * @param severityLevels Severity levels to include.
     * @return Number of matching messages.
     */
    [[nodiscard]] int getMessageCountByLevels(const util::SeverityLevels& severityLevels) const;

    /**
     * @brief Returns whether the result contains warning messages.
     * @return True if at least one warning message exists.
     */
    [[nodiscard]] bool hasWarnings() const;

    /**
     * @brief Returns whether the result contains error messages.
     * @return True if at least one error message exists.
     */
    [[nodiscard]] bool hasErrors() const;

    /**
     * @brief Returns whether the result contains fatal error messages.
     * @return True if at least one fatal error message exists.
     */
    [[nodiscard]] bool hasCriticalErrors() const;

    /**
     * @brief Counts warning messages.
     * @return Number of warning messages.
     */
    [[nodiscard]] int getWarningCount() const;

    /**
     * @brief Counts error messages.
     * @return Number of error messages.
     */
    [[nodiscard]] int getErrorCount() const;

    /**
     * @brief Counts fatal error messages.
     * @return Number of fatal error messages.
     */
    [[nodiscard]] int getFatalErrorCount() const;

public: // Metrics

    /**
     * @brief Replaces the metrics associated with this workflow result.
     * @param metrics Metrics collected during workflow execution.
     */
    void setMetrics(QVector<WorkflowMetric> metrics);

    /**
     * @brief Returns metrics collected during workflow execution.
     * @return Copy of the workflow metrics.
     */
    [[nodiscard]] QVector<WorkflowMetric> getMetrics() const;

    /**
     * @brief Returns a metric by name.
     * @param name Name of the metric to retrieve.
     * @return Matching metric, or std::nullopt when no metric with this name exists.
     */
    [[nodiscard]] std::optional<WorkflowMetric> getMetric(const QString& name) const;

private:
    std::uint64_t               _duration = 0;      /**< Workflow execution duration in milliseconds */
    WorkflowMessages            _messages;          /**< Messages emitted during workflow execution */
    QVector<WorkflowMetric>     _metrics;           /**< Metrics collected during workflow execution */
};

/** Unique ownership pointer type for workflow results. */
using UniqueWorkflowResult = std::unique_ptr<WorkflowResult>;

/** Shared ownership pointer type for workflow results. */
using SharedWorkflowResult = std::shared_ptr<WorkflowResult>;

}
