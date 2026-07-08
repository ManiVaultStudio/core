// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowExecutionMetrics.h"
#include "WorkflowExecutionOptions.h"
#include "WorkflowReportNode.h"
#include "WorkflowProgressNode.h"
#include <QUuid>
#include <QMutex>
#include <QMutexLocker>
#include <QTreeView>

namespace mv::workflow
{

/** Execution status shared across a workflow run. */
enum class WorkflowExecutionStatus {
    Idle,       /**< Workflow execution has not started */
    Running,    /**< Workflow execution is currently running */
    Finished,   /**< Workflow execution completed successfully */
    Failed      /**< Workflow execution failed */
};

/**
 * @brief Shared state for one workflow execution.
 *
 * WorkflowExecutionState owns the report and progress roots, immutable
 * execution options, lifecycle status, execution metrics, intermediate result
 * values, and published workflow outputs. It is shared by execution contexts so
 * related workflow, stage, and job scopes can coordinate through one state
 * object.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowExecutionState
{
public:
    /** Shared ownership pointer type for workflow execution state. */
    using Ptr = std::shared_ptr<WorkflowExecutionState>;

    /**
     * @brief Constructs workflow execution state.
     * @param reportRoot Root report node.
     * @param progressRoot Root progress node.
     * @param executionOptions Options used for this workflow execution.
     */
    WorkflowExecutionState(const WorkflowReportNode::SharedWorkflowReportNode& reportRoot, const WorkflowProgressNode::Ptr& progressRoot, WorkflowExecutionOptions executionOptions = {});

    /**
     * @brief Returns the root report node.
     * @return Root report node.
     */
    [[nodiscard]] WorkflowReportNode::SharedWorkflowReportNode getReportRoot() const;

    /**
     * @brief Returns the root progress node.
     * @return Root progress node.
     */
    [[nodiscard]] WorkflowProgressNode::Ptr getProgressRoot() const;

    /**
     * @brief Returns the workflow execution options.
     * @return Execution options.
     */
    [[nodiscard]] WorkflowExecutionOptions getExecutionOptions() const;

    /**
     * @brief Returns the current workflow execution status.
     * @return Execution status.
     */
    [[nodiscard]] WorkflowExecutionStatus getStatus() const;

    /**
     * @brief Sets the current workflow execution status.
     * @param status New execution status.
     */
    void setStatus(WorkflowExecutionStatus status);

    /**
     * @brief Returns the current root progress value.
     * @return Normalized workflow progress in the range [0.0, 1.0].
     */
    [[nodiscard]] double getOverallProgress() const;

    /**
     * @brief Collects messages from the report tree.
     * @return Messages from the root report node and all descendants.
     */
    [[nodiscard]] WorkflowMessages collectMessages() const;

public: // Metrics

    [[nodiscard]] WorkflowExecutionMetrics& metrics();

    [[nodiscard]] const WorkflowExecutionMetrics& metrics() const;

public: // Result values

    void publishResultValue(
        const QUuid& contextId,
        const QString& key,
        const QVariant& value);

    [[nodiscard]] QVariantMap takeResultValues(const QUuid& contextId);

public: // Workflow outputs

    void setOutput(const QUuid& id, const QVariant& value)
    {
        QMutexLocker lock(&_outputsMutex);
        _outputs[id] = value;
    }

    [[nodiscard]] QVariant takeOutput(const QUuid& id)
    {
        QMutexLocker lock(&_outputsMutex);

        auto it = _outputs.find(id);

        if (it == _outputs.end())
            return {};

        QVariant value = std::move(it.value());
        _outputs.erase(it);

        return value;
    }

private:
    static void collectMessagesRecursive(const WorkflowReportNode::SharedWorkflowReportNode& node, QVector<WorkflowMessage>& out);

private:
    WorkflowReportNode::SharedWorkflowReportNode    _reportRoot;                                /**< Root report node for this workflow execution */
    WorkflowProgressNode::Ptr                       _progressRoot;                              /**< Root progress node for this workflow execution */
    WorkflowExecutionOptions                        _executionOptions;                          /**< Options used for this workflow execution */
    mutable QMutex                                  _mutex;                                     /**< Protects mutable execution state */
    WorkflowExecutionStatus                         _status = WorkflowExecutionStatus::Idle;    /**< Current workflow execution status */
    WorkflowExecutionMetrics                        _metrics;                                   /**< Metrics collected during workflow execution */
    mutable QMutex                                  _resultValuesMutex;                         /**< Protects result values indexed by context */
    QHash<QUuid, QVariantMap>                       _resultValuesByContext;                     /**< Result values published by execution context id */
    mutable QMutex                                  _outputsMutex;                              /**< Protects published workflow outputs */
    QHash<QUuid, QVariant>                          _outputs;                                   /**< Published workflow outputs indexed by output id */
};

}
