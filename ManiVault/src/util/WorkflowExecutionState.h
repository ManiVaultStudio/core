// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowExecutionMetrics.h"
#include "WorkflowExecutionOptions.h"

namespace mv::util
{

enum class WorkflowExecutionStatus {
    Idle,
    Running,
    Finished,
    Failed
};

class CORE_EXPORT WorkflowExecutionState
{
public:
    using Ptr = std::shared_ptr<WorkflowExecutionState>;

    WorkflowExecutionState(const WorkflowReportNode::SharedWorkflowReportNode& reportRoot, const WorkflowProgressNode::Ptr& progressRoot, WorkflowExecutionOptions executionOptions = {});

    WorkflowReportNode::SharedWorkflowReportNode getReportRoot() const;

    WorkflowProgressNode::Ptr getProgressRoot() const;

    WorkflowExecutionOptions getExecutionOptions() const;

    WorkflowExecutionStatus getStatus() const;

    void setStatus(WorkflowExecutionStatus status);

    double getOverallProgress() const;

    WorkflowMessages collectMessages() const;

public: // Metrics

    WorkflowExecutionMetrics& metrics();

    const WorkflowExecutionMetrics& metrics() const;

public: // Temporary result values (key-value pairs that can be set and accessed during workflow execution

    /**
     * @brief Sets a key-value pair in the result values of the workflow execution. This allows for storing multiple pieces of information related to the workflow result in a structured way, where each piece of information can be accessed using a unique key. The value can be any QVariant, providing flexibility in the type of data that can be stored as part of the workflow result.
     * @param key The key associated with the result value, used to identify and access the value later.
     * @param value The value to be stored in the result values, which can be any QVariant representing relevant information about the workflow execution outcome.
     */
    void setResultValue(const QString& key, const QVariant& value);

    /**
     * @brief Retrieves the value associated with the specified key from the result values of the workflow execution. This function allows for accessing specific pieces of information stored in the result values using their corresponding keys, without modifying or removing the values from the result values map. If the key does not exist in the result values, this function may return an invalid QVariant or throw an exception, depending on the implementation.
     * @param key The key associated with the result value to be retrieved.
     * @return The value associated with the specified key in the result values, or an invalid QVariant if the key does not exist.
     */
    [[nodiscard]] QVariant getResultValue(const QString& key) const;

    /**
     * @brief Checks if a result value with the specified key exists in the result values of the workflow execution. This can be used to determine whether a particular piece of information has been stored in the result values and is available for retrieval before attempting to access it, allowing for safer and more robust code when working with dynamic result reporting.
     * @param key The key to check for existence in the result values.
     * @return True if a result value with the specified key exists in the result values, false otherwise.
     */
    [[nodiscard]] bool hasResultValue(const QString& key) const;

    /**
     * @brief Retrieves and removes the value associated with the specified key from the result values of the workflow execution. This function allows for consuming specific pieces of information stored in the result values using their corresponding keys, while also ensuring that once a value is taken, it will no longer be available in the result values. If the key does not exist in the result values, this function may return an invalid QVariant or throw an exception, depending on the implementation.
     * @param key The key associated with the result value to be taken.
     * @return The value associated with the specified key in the result values before it is removed, or an invalid QVariant if the key does not exist.
     */
    [[nodiscard]] QVariant takeResultValue(const QString& key);

    /**
     * @brief Retrieves all key-value pairs from the result values of the workflow execution without modifying or removing them. This function allows for accessing the entire set of information stored in the result values at once, which can be useful for scenarios where multiple pieces of information need to be accessed together or when the structure of the result values is not known in advance. The returned QVariantMap contains all key-value pairs currently stored in the result values, and any subsequent modifications to the result values will not affect the returned map.
     * @return A QVariantMap containing all key-value pairs currently stored in the result values. If there are no key-value pairs in the result values, this function may return an empty QVariantMap.
     */
    [[nodiscard]] QVariantMap getResultValues(const QString& scope);

    /**
     * @brief Retrieves and removes all key-value pairs from the result values of the workflow execution. This function allows for consuming all pieces of information stored in the result values at once, while also ensuring that once the values are taken, they will no longer be available in the result values. After calling this function, the result values will be cleared, and any subsequent attempts to access specific keys will indicate that they no longer exist.
     * @return A QVariantMap containing all key-value pairs that were in the result values before they were removed. If there were no key-value pairs in the result values, this function may return an empty QVariantMap.
     */
    [[nodiscard]] QVariantMap takeResultValues(const QString& scope);

public: // Tracing

    std::shared_ptr<AbstractWorkflowTraceSink> getTraceSink() const;

    void trace(WorkflowTraceEvent event) const;

private:
    static void collectMessagesRecursive(const WorkflowReportNode::SharedWorkflowReportNode& node, QVector<WorkflowMessage>& out);

private:
    WorkflowReportNode::SharedWorkflowReportNode    _reportRoot;                                /** Report nodes are stored in the execution state since they need to be accessible from the context and may be updated from multiple threads during execution. The execution context provides thread-safe access to these nodes, and they are designed to handle concurrent updates internally (e.g., by using mutexes). */
    WorkflowProgressNode::Ptr                       _progressRoot;                              /** Progress and report nodes are stored in the execution state since they need to be accessible from the context and may be updated from multiple threads during execution. The execution context provides thread-safe access to these nodes, and they are designed to handle concurrent updates internally (e.g., by using mutexes). */
    WorkflowExecutionOptions                        _executionOptions;                          /** Execution options are stored in the execution state since they may need to be accessed from multiple threads during execution and should be immutable after initialization. */
    mutable QMutex                                  _mutex;                                     /** Mutex to protect access to mutable members that may be updated from multiple threads during execution. */
    WorkflowExecutionStatus                         _status = WorkflowExecutionStatus::Idle;    /** The execution status is protected by a mutex since it may be updated from multiple threads during execution and needs to be read and updated atomically. */
    WorkflowExecutionMetrics                        _metrics;                                   /** Metrics are stored in the execution state since they may be updated from multiple threads during execution and need to be accessible from the context. */
    mutable QMutex                                  _resultValuesMutex;                         /** Mutex to protect access to result values that may be updated from multiple threads during execution. */
    QVariantMap                                     _resultValues;                              /** Result values are stored in the execution state since they may be updated from multiple threads during execution and need to be accessible from the context. The result values map is protected by a mutex to ensure thread-safe access and updates. */
};

}