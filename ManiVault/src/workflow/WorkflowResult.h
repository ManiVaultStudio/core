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
 * @brief Result of a complete workflow execution.
 *
 * WorkflowResult extends WorkflowResultBase with execution duration, collected
 * workflow messages, and metrics captured during the run.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowResult final : public WorkflowResultBase
{
public:

	/**
     * @brief Constructs a WorkflowResult object with the specified workflow name. The workflow name is used for identification and logging purposes, allowing you to associate the result with a specific workflow execution.
	 * @param workflowName The name of the workflow that produced this result. This can be used for identification and logging purposes, allowing you to associate the result with a specific workflow execution.
	 */
	WorkflowResult(const QString& workflowName);

public: // General

    /**
     * @brief Retrieves the duration of the workflow execution in milliseconds.
     * @return The duration of the workflow execution in milliseconds.
     */
    std::uint64_t getDurationMS() const;

    /**
     * @brief Sets the duration of the workflow execution in milliseconds.
     * @param duration The duration to be set for the workflow execution in milliseconds.
     */
    void setDurationMS(std::uint64_t duration);

public: // Messages

    /** @return All messages collected during workflow execution. */
    WorkflowMessages getMessages() const;

    /** @return Messages with warning severity. */
    WorkflowMessages getWarningMessages() const;

    /** @return Messages with error severity. */
    WorkflowMessages getErrorMessages() const;

    /** @return Messages with fatal severity. */
    WorkflowMessages getFatalErrorMessages() const;

    /** Sets the messages collected during workflow execution. */
    void             setMessages(const WorkflowMessages& workflowMessages);

    /** Returns messages whose severity is contained in the supplied set. */
    WorkflowMessages getMessagesByLevels(const util::SeverityLevels& severityLevels) const;

    /** Returns the number of messages whose severity is contained in the supplied set. */
    int              getMessageCountByLevels(const util::SeverityLevels& severityLevels) const;

    /** @return True when this result contains warning messages. */
    bool             hasWarnings() const;

    /** @return True when this result contains error messages. */
    bool             hasErrors() const;

    /** @return True when this result contains fatal error messages. */
    bool             hasCriticalErrors() const;

    /** @return Number of warning messages. */
    int              getWarningCount() const;

    /** @return Number of error messages. */
    int              getErrorCount() const;

    /** @return Number of fatal error messages. */
    int              getFatalErrorCount() const;

public: // Metrics

    /**
     * @brief Sets the metrics associated with this workflow result. Metrics are quantitative measurements that can be used to evaluate the performance, efficiency, or other relevant aspects of the workflow execution. By setting metrics for a workflow result, you can provide additional insights and data points that can be used for analysis, monitoring, and optimization of the workflow execution. The metrics can include various types of measurements such as execution time, resource usage, throughput, or any other relevant performance indicators that are meaningful in the context of the workflow being executed.
     * @param metrics The vector of WorkflowMetric objects to be associated with this workflow result. Each WorkflowMetric object represents a specific metric with its name, value, and optional details. By providing a vector of metrics, you can include multiple measurements that capture different aspects of the workflow execution, allowing for a comprehensive evaluation and analysis of the workflow's performance and outcomes.
     */
    void setMetrics(QVector<WorkflowMetric> metrics);

    /**
     * @brief Retrieves the metrics associated with this workflow result. Metrics are quantitative measurements that can be used to evaluate the performance, efficiency, or other relevant aspects of the workflow execution. By retrieving the metrics for a workflow result, you can access the additional insights and data points that were set for this result, which can be used for analysis, monitoring, and optimization of the workflow execution. The metrics can include various types of measurements such as execution time, resource usage, throughput, or any other relevant performance indicators that are meaningful in the context of the workflow being executed.
     * @return A vector of WorkflowMetric objects associated with this workflow result. Each WorkflowMetric object represents a specific metric with its name, value, and optional details. By providing a vector of metrics, you can access multiple measurements that capture different aspects of the workflow execution, allowing for a comprehensive evaluation and analysis of the workflow's performance and outcomes.
     */
    QVector<WorkflowMetric> getMetrics() const;

    /**
     * @brief Retrieves a specific metric associated with this workflow result by its name. Metrics are quantitative measurements that can be used to evaluate the performance, efficiency, or other relevant aspects of the workflow execution. By retrieving a specific metric by its name, you can access a particular measurement that is relevant for analysis, monitoring, or optimization of the workflow execution. The metric can include various types of measurements such as execution time, resource usage, throughput, or any other relevant performance indicators that are meaningful in the context of the workflow being executed.
     * @param name The name of the metric to be retrieved. This should correspond to the name of a metric that was set for this workflow result. By providing the name of the metric, you can access the specific measurement that you are interested in, allowing for targeted analysis and evaluation of the workflow's performance and outcomes.
     * @return An optional containing the WorkflowMetric object with the specified name if it exists, or an empty optional if no such metric is found. By returning an optional, this method allows for safe retrieval of metrics without risking errors or undefined behavior if the requested metric does not exist.
     */
    std::optional<WorkflowMetric> getMetric(const QString& name) const;

private:

    std::uint64_t               _duration = 0;      /**< Workflow execution duration in milliseconds */
    WorkflowMessages            _messages;          /**< Messages collected during workflow execution */
    QVector<WorkflowMetric>     _metrics;           /**< Metrics captured during workflow execution */
};

using UniqueWorkflowResult = std::unique_ptr<WorkflowResult>;
using SharedWorkflowResult = std::shared_ptr<WorkflowResult>;

}
