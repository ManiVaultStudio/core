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

    /** Represents the status of a workflow result. This can be used to indicate whether the workflow execution was successful, failed, or canceled. */
    enum class Status {
        Success,        /** The workflow execution completed successfully. */
        Failed,         /** The workflow execution encountered an error and failed. */
        Canceled        /** The workflow execution was canceled before completion. */
    };

public:

    WorkflowResult(const QString& workflowName);

public: // General

    QString getWorkflowName() const;

    /**
     * @brief Retrieves the status of the workflow result. This indicates whether the workflow execution was successful, failed, or canceled.
     * @return The status of the workflow result, which can be Success, Failed, or Canceled.
     */
    Status getStatus() const;

    /**
     * @brief Sets the status of the workflow result. This can be used to indicate whether the workflow execution was successful, failed, or canceled.
     * @param status The status to be set for the workflow result, which can be Success, Failed, or Canceled.
     */
    void setStatus(Status status);

    /**
     * @brief Retrieves the duration of the workflow execution in milliseconds.
     * @return The duration of the workflow execution in milliseconds.
     */
    std::uint64_t getDuration() const;

    /**
     * @brief Sets the duration of the workflow execution in milliseconds.
     * @param duration The duration to be set for the workflow execution in milliseconds.
     */
    void setDuration(std::uint64_t duration);

public: // Messages

    WorkflowMessages getMessages() const;
    WorkflowMessages getWarningMessages() const;
    WorkflowMessages getErrorMessages() const;
    WorkflowMessages getFatalErrorMessages() const;
    void setMessages(const WorkflowMessages& workflowMessages);
    WorkflowMessages getMessagesByLevels(const SeverityLevels& severityLevels) const;
    int getMessageCountByLevels(const SeverityLevels& severityLevels) const;
    bool hasWarnings() const;
    bool hasErrors() const;
    bool hasCriticalErrors() const;
    int getWarningCount() const;
    int getErrorCount() const;
    int getFatalErrorCount() const;

public: // Value

    /**
     * @brief Checks if the workflow result contains a valid value. This can be used to determine whether the workflow execution produced a meaningful output that can be further processed or utilized. If this method returns false, it indicates that the workflow did not produce a valid result, and any attempt to access the value may result in an error or undefined behavior.
     * @return True if the workflow result contains a valid value, false otherwise.
     */
    bool hasValue() const;

    /**
     * @brief Retrieves the value produced by the workflow execution. This can be any type of data that can be stored in a QVariant, such as a result object, a data structure, or any other relevant output from the workflow. The caller can use this value to further process or utilize the results of the workflow execution. It is important to check if the value is valid using the hasValue() method before attempting to access it, to avoid potential errors or undefined behavior.
     * @return The value produced by the workflow execution.
     */
    const QVariant& getValue() const;

    /**
     * @brief Sets the value produced by the workflow execution. This can be any type of data that can be stored in a QVariant, such as a result object, a data structure, or any other relevant output from the workflow. The caller can use this method to store the results of the workflow execution, which can then be accessed by parent contexts or other parts of the workflow as needed. It is important to ensure that the value being set is valid and meaningful for the context of the workflow execution.
     * @param value The value to be set for the workflow result.
     */
    void setValue(const QVariant& value);

    /**
     * @brief Sets the value produced by the workflow execution using move semantics. This can be any type of data that can be stored in a QVariant, such as a result object, a data structure, or any other relevant output from the workflow. The caller can use this method to store the results of the workflow execution, which can then be accessed by parent contexts or other parts of the workflow as needed. It is important to ensure that the value being set is valid and meaningful for the context of the workflow execution.
     * @param value The value to be set for the workflow result.
     */
    void setValue(QVariant&& value);

    /**
     * @brief Sets the value produced by the workflow execution using perfect forwarding. This can be any type of data that can be stored in a QVariant, such as a result object, a data structure, or any other relevant output from the workflow. The caller can use this method to store the results of the workflow execution, which can then be accessed by parent contexts or other parts of the workflow as needed. It is important to ensure that the value being set is valid and meaningful for the context of the workflow execution.
     * @tparam T The type of the value being set.
     * @param value The value to be set for the workflow result.
     */
    template<typename T>
    void setValue(T&& value) {
        _value = QVariant::fromValue(std::forward<T>(value));
    }

    /**
     * @brief Checks if the value produced by the workflow execution can be converted to a specified type T. This can be used to determine if the value can be safely accessed and utilized as a specific type without causing errors or undefined behavior. If this method returns true, it indicates that the value can be converted to the specified type T, and the caller can proceed to access the value using the value<T>() method. If it returns false, it indicates that the value cannot be converted to the specified type T, and any attempt to access it as that type may result in an error or undefined behavior.
     * @tparam T The type to which the value is being checked for conversion.
     * @return True if the value can be converted to the specified type T, false otherwise.
     */
    template<typename T>
    bool canConvert() const {
        return _value.canConvert<T>();
    }

    /**
     * @brief Retrieves the value produced by the workflow execution as a specified type T. This can be used to access the value in a specific type that is relevant for further processing or utilization. It is important to check if the value can be converted to the specified type T using the canConvert<T>() method before attempting to access it, to avoid potential errors or undefined behavior. If the value cannot be converted to the specified type T, this method may throw an exception or result in an error.
     * @tparam T The type to which the value is being accessed.
     * @return The value produced by the workflow execution as a specified type T.
     */
    template<typename T>
    T value() const {
        return _value.value<T>();
    }

    /**
     * @brief Attempts to retrieve the value produced by the workflow execution as a specified type T. This can be used to safely access the value in a specific type that is relevant for further processing or utilization, while avoiding potential errors or undefined behavior. If the value can be converted to the specified type T, this method returns an optional containing the value as that type. If the value cannot be converted to the specified type T, this method returns an empty optional, indicating that the value cannot be accessed as that type.
     * @tparam T The type to which the value is being accessed.
     * @return An optional containing the value produced by the workflow execution as a specified type T if it can be converted, or an empty optional if it cannot be converted.
     */
    template<typename T>
    std::optional<T> tryValue() const
    {
        if (!_value.isValid() || !_value.canConvert<T>())
            return std::nullopt;

        return _value.value<T>();
    }

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
    QString                     _workflowName;                  /** The name of the workflow that produced this result. This can be used for identification and logging purposes. */
    std::uint64_t               _duration = 0;                  /** The duration of the workflow execution in milliseconds. This can be used for performance monitoring and analysis. */
    Status                      _status = Status::Success;      /** The status of the workflow execution, indicating whether it was successful, failed, or canceled. This can be used to determine the overall outcome of the workflow execution. */
    QVariant                    _value;                         /** The value produced by the workflow execution. This can be any type of data that can be stored in a QVariant, such as a result object, a data structure, or any other relevant output from the workflow. */
    WorkflowMessages            _messages;                      /** The messages generated during the workflow execution. This can include informational messages, warnings, errors, and critical errors that occurred during the execution. These messages can be used for debugging, logging, and providing feedback to users about the workflow execution. */
    QVector<WorkflowMetric>     _metrics;
};

using UniqueWorkflowResult = std::unique_ptr<WorkflowResult>;
using SharedWorkflowResult = std::shared_ptr<WorkflowResult>;

} // namespace mv::util