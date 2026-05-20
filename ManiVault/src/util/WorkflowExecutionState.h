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

public: // Result

    /**
     * @brief Sets the result of the workflow execution. The result can be any QVariant that represents the outcome of the workflow, such as a data structure, a file path, or any other relevant information that needs to be returned after the workflow execution is completed. The result is stored in the execution state and can be accessed from the workflow execution context.
     * @param result The result of the workflow execution.
     */
    void setResult(QVariant result) {
        _result = std::move(result);
    }

    /**
     * @brief Retrieves and clears the result of the workflow execution. This function returns the current result stored in the execution state and then clears it to ensure that subsequent calls will not return the same result again. This is useful for scenarios where the result should only be consumed once, such as when passing the result to another component or when finalizing the workflow execution.
     * @return The current result of the workflow execution before it is cleared.
     */
    QVariant takeResult() {
        return std::exchange(_result, {});
    }

public: // Tracing

    std::shared_ptr<AbstractWorkflowTraceSink> getTraceSink() const;

    void trace(WorkflowTraceEvent event) const;

private:
    static void collectMessagesRecursive(const WorkflowReportNode::SharedWorkflowReportNode& node, QVector<WorkflowMessage>& out);

private:
    WorkflowReportNode::SharedWorkflowReportNode     _reportRoot;                                /** Report nodes are stored in the execution state since they need to be accessible from the context and may be updated from multiple threads during execution. The execution context provides thread-safe access to these nodes, and they are designed to handle concurrent updates internally (e.g., by using mutexes). */
    WorkflowProgressNode::Ptr   _progressRoot;                              /** Progress and report nodes are stored in the execution state since they need to be accessible from the context and may be updated from multiple threads during execution. The execution context provides thread-safe access to these nodes, and they are designed to handle concurrent updates internally (e.g., by using mutexes). */
    WorkflowExecutionOptions    _executionOptions;                          /** Execution options are stored in the execution state since they may need to be accessed from multiple threads during execution and should be immutable after initialization. */
    mutable QMutex              _mutex;                                     /** Mutex to protect access to mutable members that may be updated from multiple threads during execution. */
    WorkflowExecutionStatus     _status = WorkflowExecutionStatus::Idle;    /** The execution status is protected by a mutex since it may be updated from multiple threads during execution and needs to be read and updated atomically. */
    WorkflowExecutionMetrics    _metrics;                                   /** Metrics are stored in the execution state since they may be updated from multiple threads during execution and need to be accessible from the context. */
    QVariant                    _result;
};

} // namespace mv::util