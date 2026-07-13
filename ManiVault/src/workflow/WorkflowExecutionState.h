// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowExecutionMetrics.h"
#include "WorkflowOptions.h"
#include "WorkflowReportNode.h"
#include "WorkflowProgressNode.h"
#include <QUuid>
#include <QHash>
#include <QMutex>
#include <QMutexLocker>
#include <QVariant>
#include <QVariantMap>

namespace mv::workflow
{

/**
 * @brief Overall lifecycle status of a workflow execution.
 */
enum class WorkflowExecutionStatus {
    Idle,       /**< Workflow has not started */
    Running,    /**< Workflow is currently executing */
    Finished,   /**< Workflow completed successfully */
    Failed      /**< Workflow failed during execution */
};

/**
 * @brief Shared mutable state for a workflow execution tree.
 *
 * WorkflowExecutionState owns the root report and progress nodes, execution
 * options, aggregate metrics, execution status, published result values, and
 * routed workflow outputs. It is shared by all WorkflowExecutionContext objects
 * that belong to one root workflow execution.
 *
 * The state protects mutable maps and status fields because workflow stages and
 * jobs may update them from different worker threads after the execution
 * refactor.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowExecutionState
{
public:

    /** Shared pointer type for workflow execution state objects. */
    using Ptr = std::shared_ptr<WorkflowExecutionState>;

    /**
     * @brief Constructs shared workflow execution state.
     * @param reportRoot Root report node for lifecycle and diagnostic messages.
     * @param progressRoot Root progress node for progress aggregation.
     * @param options Workflow execution options.
     */
    WorkflowExecutionState(const WorkflowReportNode::SharedWorkflowReportNode& reportRoot, const WorkflowProgressNode::Ptr& progressRoot, WorkflowOptions options = {});

    /** @return Root report node for the workflow execution. */
    [[nodiscard]] WorkflowReportNode::SharedWorkflowReportNode getReportRoot() const;

    /** @return Root progress node for the workflow execution. */
    [[nodiscard]] WorkflowProgressNode::Ptr getProgressRoot() const;

    /** @return Workflow options captured for this execution. */
    [[nodiscard]] WorkflowOptions getOptions() const;

    /** @return Current workflow execution status. */
    [[nodiscard]] WorkflowExecutionStatus getStatus() const;

    /**
     * @brief Updates the workflow execution status.
     * @param status New workflow execution status.
     */
    void setStatus(WorkflowExecutionStatus status);

    /** @return Aggregated progress of the root progress node. */
    [[nodiscard]] double getOverallProgress() const;

    /** @return Messages collected recursively from the report tree. */
    [[nodiscard]] WorkflowMessages collectMessages() const;

public: // Metrics

    /** @return Mutable execution metrics accumulator. */
    [[nodiscard]] WorkflowExecutionMetrics& metrics();

    /** @return Const execution metrics accumulator. */
    [[nodiscard]] const WorkflowExecutionMetrics& metrics() const;

public: // Result values

    /**
     * @brief Publishes a named result value for an execution context.
     * @param contextId Identifier of the context that owns the value.
     * @param key Value key within the context result map.
     * @param value Value to store.
     */
    void publishResultValue(
        const QUuid& contextId,
        const QString& key,
        const QVariant& value);

    /**
     * @brief Takes and removes all published result values for a context.
     * @param contextId Identifier of the context whose values should be taken.
     * @return Result values published for the context.
     */
    [[nodiscard]] QVariantMap takeResultValues(const QUuid& contextId);

public: // Workflow outputs

    /**
     * @brief Stores an output value under an output identifier.
     * @param id Output identifier, typically from a WorkflowHandle or context output id.
     * @param value Output value to store.
     */
    void setOutput(const QUuid& id, const QVariant& value)
    {
        QMutexLocker lock(&_outputsMutex);
        _outputs[id] = value;
    }

    /**
     * @brief Takes and removes an output value.
     * @param id Output identifier to read.
     * @return Stored output value, or an invalid QVariant when no output exists.
     */
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

    /**
     * @brief Recursively collects report messages from a report tree.
     * @param node Report node to visit.
     * @param out Output message collection.
     */
    static void collectMessagesRecursive(const WorkflowReportNode::SharedWorkflowReportNode& node, QVector<WorkflowMessage>& out);

private:

    WorkflowReportNode::SharedWorkflowReportNode    _reportRoot;                /**< Root report tree shared by all execution contexts */
    WorkflowProgressNode::Ptr                       _progressRoot;              /**< Root progress tree shared by all execution contexts */
    WorkflowOptions                                 _options;                   /**< Options captured for this workflow execution */
    mutable QMutex                                  _mutex;                     /**< Protects mutable execution status */
    WorkflowExecutionStatus                         _status = WorkflowExecutionStatus::Idle;    /**< Current execution status */
    WorkflowExecutionMetrics                        _metrics;                   /**< Aggregate execution metrics */
    mutable QMutex                                  _resultValuesMutex;         /**< Protects context result values */
    QHash<QUuid, QVariantMap>                       _resultValuesByContext;     /**< Published result values indexed by context id */
    mutable QMutex                                  _outputsMutex;              /**< Protects routed workflow outputs */
    QHash<QUuid, QVariant>                          _outputs;                   /**< Output values indexed by output id */
};

}
