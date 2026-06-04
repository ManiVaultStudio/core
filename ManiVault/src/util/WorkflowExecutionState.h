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

    [[nodiscard]] WorkflowReportNode::SharedWorkflowReportNode getReportRoot() const;

    [[nodiscard]] WorkflowProgressNode::Ptr getProgressRoot() const;

    [[nodiscard]] WorkflowExecutionOptions getExecutionOptions() const;
    [[nodiscard]] WorkflowExecutionStatus getStatus() const;

    void setStatus(WorkflowExecutionStatus status);

    [[nodiscard]] double getOverallProgress() const;
    [[nodiscard]] WorkflowMessages collectMessages() const;

public: // Metrics

    [[nodiscard]] WorkflowExecutionMetrics& metrics();

    [[nodiscard]] const WorkflowExecutionMetrics& metrics() const;

public: // Tracing

    [[nodiscard]] std::shared_ptr<AbstractWorkflowTraceSink> getTraceSink() const;

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
};

}