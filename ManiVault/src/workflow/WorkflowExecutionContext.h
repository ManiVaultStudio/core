// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowReportNode.h"
#include "WorkflowProgressNode.h"
#include "WorkflowExecutionState.h"
#include "WorkflowPlan.h"
#include "WorkflowStageSummary.h"
#include "WorkflowExecutionNodeType.h"
#include "Task.h"

#include <QString>
#include <QUuid>
#include <QMutex>
#include <QMutexLocker>
#include <QTreeView>


namespace mv::workflow
{

class CORE_EXPORT WorkflowExecutionContext : public std::enable_shared_from_this<WorkflowExecutionContext>
{
public:

    /** Shared report node pointer type used by execution contexts. */
    using ReportNodePtr = WorkflowReportNode::SharedWorkflowReportNode;

    /** Shared progress node pointer type used by execution contexts. */
    using ProgressNodePtr = WorkflowProgressNode::Ptr;

    /** Shared execution state pointer type used by execution contexts. */
    using StatePtr = WorkflowExecutionState::Ptr;

    /** Semantic execution node type. */
    using Type = WorkflowExecutionNodeType;

    /** Constructs an empty workflow execution context. */
    WorkflowExecutionContext();

    /**
     * @brief Constructs a workflow execution context.
     * @param name Human-readable context name.
     * @param reportNode Report node associated with this context.
     * @param progressNode Progress node associated with this context.
     * @param state Shared execution state.
     * @param task Optional task used for GUI progress reporting.
     * @param progressMode Progress aggregation mode for this context.
     */
    WorkflowExecutionContext(QString name, ReportNodePtr reportNode, ProgressNodePtr progressNode, StatePtr state, Task* task = nullptr, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic);

    /** @return Semantic execution node type for this context. */
    Type getType() const
    {
        return _type;
    }

    /**
     * @brief Sets the semantic execution node type.
     * @param type New semantic execution node type.
     */
    void setType(Type type)
    {
        _type = type;
    }

    /**
     * @brief Creates a root workflow execution context.
     * @param name Root workflow name.
     * @param task Optional task used for progress reporting.
     * @param options Workflow execution options.
     * @return Root execution context.
     */
    static SharedWorkflowExecutionContext makeRoot(const QString& name, Task* task, WorkflowOptions options = {});

    /**
     * @brief Creates a child context of the requested semantic type.
     * @param type Semantic type of the child context.
     * @param name Child context name.
     * @param weight Relative progress contribution of the child.
     * @param progressMode Progress aggregation mode for the child.
     * @return Child execution context, or nullptr if this context is invalid.
     */
    SharedWorkflowExecutionContext createChild(Type type, const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic);

    /**
     * @brief Creates a workflow child context.
     *
     * This function creates a new workflow execution context that inherits execution state,
     * reporting infrastructure, progress tracking infrastructure, and task association from
     * the current context while establishing a new execution hierarchy level for a child workflow.
     *
     * The created child context:
     * - Shares the same workflow execution state
     * - Shares the same task association
     * - Extends the execution path hierarchy
     * - Creates child report and progress nodes
     * - Establishes a unique result scope
     * - Receives its own unique execution identifier
     * - Is classified as a Workflow execution context
     *
     * This function is typically used when:
     * - Executing a root workflow from another workflow executor
     * - Creating logical workflow boundaries
     * - Constructing nested execution hierarchies
     * - Establishing workflow-level progress aggregation
     *
     * @param name Human-readable name of the child workflow context.
     * @param weight Relative progress contribution weight of the child workflow.
     * @param progressMode Progress reporting mode used by the child workflow.
     * @return Shared pointer to the newly created workflow execution context.
     */
    SharedWorkflowExecutionContext createWorkflowChild(const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic);

    /** Creates a nested workflow child context. */
    SharedWorkflowExecutionContext createNestedWorkflowChild(const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic);

    /** Creates a sequential stage child context. */
    SharedWorkflowExecutionContext createSequentialStageChild(const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic);

    /** Creates a parallel stage child context. */
    SharedWorkflowExecutionContext createParallelStageChild(const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic);

    /** Creates a job child context. */
    SharedWorkflowExecutionContext createJobChild(const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic);

    /** Creates a typed child context with an explicit progress weight and mode. */
    SharedWorkflowExecutionContext createTypedChild(Type type, const QString& name, double weight, WorkflowPlan::JobProgressMode progressMode);

    /** @return True when this context has no parent context. */
    bool isRootExecution() const;

public:

    /** Reports this context as started. */
    void reportStarted() const;

    /** Reports this context as finished. */
    void reportFinished(std::uint64_t durationMs = 0);

    /** Reports this context as failed. */
    void reportFailed(util::SeverityLevel severity, const QString& errorMessage, QVariantMap extraDetails = {});

    /** Reports this context as skipped. */
    void reportSkipped(const QString& reason);

    /** Reports a stage summary on this context. */
    void reportStageSummary(const WorkflowStageSummary& summary) const;

    /**
     * @brief Builds standard lifecycle details for report messages.
     * @param event Lifecycle event name.
     * @param durationMs Optional event duration in milliseconds.
     * @return Variant map with context identity, hierarchy, and timing details.
     */
    QVariantMap makeLifecycleDetails(const QString& event, std::uint64_t durationMs = 0) const;

public:

    /** @return True when the progress node has child progress nodes. */
    bool hasProgressChildren() const;

    /** @return True when this context has report, progress, and state objects. */
    bool isValid() const;

    /** @return Human-readable context name. */
    QString getName() const;

    /** Adds a message with an explicit severity to the report node and console output. */
    void message(util::SeverityLevel severity, QString text, QString location, QVariantMap details) const;

    /**
     * @brief Adds an informational message.
     *
     * @param text The main text or content of the informational message to be added to the report node.
     * @param location The specific location in the code or workflow where the informational message was generated (e.g., function name, line number). This can be used to provide additional context for the message and help with debugging and tracing.
     * @param details Additional details or metadata associated with the informational message (optional). This can be used to provide further context or structured information related to the message, such as variable values, execution state, or other relevant data that may assist in understanding the message and its implications within the workflow execution.
     */
    void info(QString text, QString location = {}, QVariantMap details = {}) const;

    /**
     * @brief Adds a warning message.
     *
     * @param text The main text or content of the warning message to be added to the report node.
     * @param location The specific location in the code or workflow where the warning message was generated (e.g., function name, line number). This can be used to provide additional context for the message and help with debugging and tracing.
     * @param details Additional details or metadata associated with the warning message (optional). This can be used to provide further context or structured information related to the message, such as variable values, execution state, or other relevant data that may assist in understanding the message and its implications within the workflow execution.
     */
    void warning(QString text, QString location = {}, QVariantMap details = {}) const;

    /**
     * @brief Adds an error message.
     *
     * @param text The main text or content of the error message to be added to the report node.
     * @param location The specific location in the code or workflow where the error message was generated (e.g., function name, line number). This can be used to provide additional context for the message and help with debugging and tracing.
     * @param details Additional details or metadata associated with the error message (optional). This can be used to provide further context or structured information related to the message, such as variable values, execution state, or other relevant data that may assist in understanding the message and its implications within the workflow execution.
     */
    void error(QString text, QString location = {}, QVariantMap details = {}) const;

    /** Marks this context as failed in the progress node and synchronizes task progress. */
    void markFailed();

    /** Sets this context progress and synchronizes task progress. */
    void setProgress(double value) const;

    /** @return Current progress value for this context. */
    double getProgress() const;

    /** @return Report node associated with this context. */
    ReportNodePtr getReportNode() const;

    /** @return Progress node associated with this context. */
    ProgressNodePtr getProgressNode() const;

    /** @return Shared execution state associated with this context. */
    StatePtr getState() const;

    /** @return Progress aggregation mode for this context. */
    WorkflowPlan::JobProgressMode getProgressMode() const;

    /**
     * @brief Gets the execution path.
     *
     * The path is built from this context and its ancestors, starting at the
     * root context.
     *
     * @param separator The string to use as a separator between levels of the execution path. The default value is " / ", but it can be customized to use any other separator as needed (e.g., " > ", " -> ", etc.).
     * @return The execution path of this workflow execution context as a string, with each level of the hierarchy separated by the specified separator.
     */
    QString getExecutionPath(const QString& separator = "/") const;

    /**
     * @brief Gets the hierarchical depth.
     *
     * The depth represents how deeply nested this context is relative to the root workflow execution context.
     * A root workflow execution context has a depth of 0, its direct children have a depth of 1, and so on.
     *
     * The depth is derived from the execution path associated with this context. Since the execution path
     * always contains at least the name of the current context itself, the depth is computed as:
     *
     *     executionPath.size() - 1
     *
     * This value is primarily useful for:
     * - Rendering hierarchical console or log output with indentation
     * - Visualizing nested workflows, stages, and jobs
     * - Debugging workflow execution structure
     * - Computing relative execution scope information
     *
     * @return The hierarchical depth of this workflow execution context, where 0 represents the root context.
     */
    std::int32_t getDepth() const;

    /** @return Workflow options from the shared execution state, or defaults if unavailable. */
    WorkflowOptions getOptions() const;

public: // ID

    /**
     * Get the unique identifier of this workflow execution context. This ID is used for tracing and logging purposes, and can be used to correlate log messages and trace events with specific workflow executions.
     * @return The unique identifier of this workflow execution context.
     */
    QUuid getId() const;

    /**
     * @brief Sets the output identifier used by setOutput() and takeOutput().
     * @param outputId Output identifier to route values through shared state.
     */
    void setOutputId(const QUuid& outputId);

    /** @return Output identifier used by this context. */
    QUuid getOutputId() const;

    /**
     * Get the unique identifier of the parent workflow execution context, if any. This can be used to correlate log messages and trace events with parent-child relationships between workflow executions.
     * @return The unique identifier of the parent workflow execution context, or a null QUuid if this is a root context.
     */
    QUuid getParentId() const;

public: // Result values

    /** @return Task associated with this context, or nullptr. */
    Task* getTask() const;

public: // Child context and output  management

    /**
     * @brief Registers a named child context.
     * @param name Child context lookup name.
     * @param child Child context to register.
     */
    void registerChildContext(const QString& name, const SharedWorkflowExecutionContext& child);

    /**
     * @brief Retrieves a named child context.
     * @param name Child context lookup name.
     * @return Registered child context, or nullptr.
     */
    [[nodiscard]] SharedWorkflowExecutionContext getChildContext(const QString& name) const;

    /**
     * @brief Stores an output value for this context output id.
     * @param value Output value to publish through the shared execution state.
     */
    void setOutput(const QVariant& value);

    /** @return Output value for this context output id, removed from shared state. */
    [[nodiscard]] QVariant takeOutput();

    /**
     * @brief Takes an output produced for a workflow handle.
     * @param handle Workflow handle whose id identifies the output.
     * @return Output value, or an invalid QVariant when unavailable.
     */
    QVariant takeOutput(const WorkflowHandle& handle);

    /** @return Parent execution context, or nullptr for root contexts. */
    [[nodiscard]] SharedWorkflowExecutionContext getParent() const;

    /** @return Names of registered child contexts. */
    [[nodiscard]] QStringList getChildNames() const;

    /** @return True when the output id differs from this context id. */
    bool hasExplicitOutputId() const;

private:

    /** Synchronizes the GUI task progress from the shared execution state. */
    void syncTaskProgress() const;

private:

    friend class WorkflowExecutionScope;

private:

    QString                                         _name;                                                      /**< Human-readable execution context name */
    QUuid                                           _id;                                                        /**< Unique execution context identifier */
    QUuid                                           _outputId;                                                  /**< Identifier used to route output values */
    QUuid                                           _parentId;                                                  /**< Unique identifier of the parent context, if any */
    QStringList                                     _executionPath;                                             /**< Hierarchical execution path from the root context */
    ReportNodePtr                                   _reportNode;                                                /**< Report node associated with this context */
    ProgressNodePtr                                 _progressNode;                                              /**< Progress node associated with this context */
    StatePtr                                        _state;                                                     /**< Shared execution state for the root workflow */
    QPointer<Task>                                  _task;                                                      /**< Task used for GUI progress reporting */
    WorkflowPlan::JobProgressMode                   _progressMode = WorkflowPlan::JobProgressMode::Automatic;   /**< Progress aggregation mode */
    Type                                            _type = Type::Workflow;                                     /**< Semantic execution node type */
    std::weak_ptr<WorkflowExecutionContext>         _parent;                                                    /**< Parent execution context */
    mutable QMutex                                  _childrenMutex;                                             /**< Protects child context lookup */
    QHash<QString, SharedWorkflowExecutionContext>  _childrenByName;                                            /**< Child contexts indexed by name */
};

/** Shared reference to a WorkflowExecutionContext. */
using SharedWorkflowExecutionContext = std::shared_ptr<WorkflowExecutionContext>;

}
