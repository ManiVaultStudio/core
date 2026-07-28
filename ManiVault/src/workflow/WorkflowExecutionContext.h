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

/**
 * @brief Runtime context for one workflow execution node.
 *
 * WorkflowExecutionContext binds together the report node, progress node,
 * shared execution state, optional task, and hierarchy metadata for a workflow,
 * stage, or job. Contexts form a tree that mirrors the workflow plan during
 * execution.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
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

    /**
     * @brief Constructs an empty workflow execution context.
     */
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

    /**
     * @brief Returns the semantic execution node type.
     * @return Semantic execution node type for this context.
     */
    [[nodiscard]] Type getType() const
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
    [[nodiscard]] SharedWorkflowExecutionContext createChild(Type type, const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic);

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
     * @param name Human-readable name of the child workflow context.
     * @param weight Relative progress contribution weight of the child workflow.
     * @param progressMode Progress reporting mode used by the child workflow.
     * @return Shared pointer to the newly created workflow execution context.
     */
    [[nodiscard]] SharedWorkflowExecutionContext createWorkflowChild(const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic);

    /**
     * @brief Creates a nested workflow child context.
     * @param name Human-readable child context name.
     * @param weight Relative progress contribution of the child.
     * @param progressMode Progress aggregation mode for the child.
     * @return Nested workflow child context.
     */
    [[nodiscard]] SharedWorkflowExecutionContext createNestedWorkflowChild(const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic);

    /**
     * @brief Creates a sequential stage child context.
     * @param name Human-readable child context name.
     * @param weight Relative progress contribution of the child.
     * @param progressMode Progress aggregation mode for the child.
     * @return Sequential stage child context.
     */
    [[nodiscard]] SharedWorkflowExecutionContext createSequentialStageChild(const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic);

    /**
     * @brief Creates a parallel stage child context.
     * @param name Human-readable child context name.
     * @param weight Relative progress contribution of the child.
     * @param progressMode Progress aggregation mode for the child.
     * @return Parallel stage child context.
     */
    [[nodiscard]] SharedWorkflowExecutionContext createParallelStageChild(const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic);

    /**
     * @brief Creates a job child context.
     * @param name Human-readable child context name.
     * @param weight Relative progress contribution of the child.
     * @param progressMode Progress aggregation mode for the child.
     * @return Job child context.
     */
    [[nodiscard]] SharedWorkflowExecutionContext createJobChild(const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic);

    /**
     * @brief Creates a typed child context.
     * @param type Semantic child context type.
     * @param name Human-readable child context name.
     * @param weight Relative progress contribution of the child.
     * @param progressMode Progress aggregation mode for the child.
     * @return Typed child execution context.
     */
    [[nodiscard]] SharedWorkflowExecutionContext createTypedChild(Type type, const QString& name, double weight, WorkflowPlan::JobProgressMode progressMode);

    /**
     * @brief Returns whether this context has no parent.
     * @return True for root execution contexts.
     */
    [[nodiscard]] bool isRootExecution() const;

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
    [[nodiscard]] QVariantMap makeLifecycleDetails(const QString& event, std::uint64_t durationMs = 0) const;

public:

    /** @return True when the progress node has child progress nodes. */
    [[nodiscard]] bool hasProgressChildren() const;

    /** @return True when this context has report, progress, and state objects. */
    [[nodiscard]] bool isValid() const;

    /** @return Human-readable context name. */
    [[nodiscard]] QString getName() const;

    /** Adds a message with an explicit severity to the report node and console output. */
    void message(util::SeverityLevel severity, QString text, QString location, QVariantMap details) const;

    /**
     * @brief Adds an informational message.
     *
     * @param text Message text.
     * @param location Source or workflow location associated with the message.
     * @param details Additional structured details.
     * @param kind Message category.
     */
    void info(QString text, QString location = {}, QVariantMap details = {}, MessageKind kind = MessageKind::Diagnostic) const;

    /**
     * @brief Adds a warning message.
     *
     * @param text Message text.
     * @param location Source or workflow location associated with the message.
     * @param details Additional structured details.
     * @param kind Message category.
     */
    void warning(QString text, QString location = {}, QVariantMap details = {}, MessageKind kind = MessageKind::Diagnostic) const;

    /**
     * @brief Adds an error message.
     *
     * @param text Message text.
     * @param location Source or workflow location associated with the message.
     * @param details Additional structured details.
     * @param kind Message category.
     */
    void error(QString text, QString location = {}, QVariantMap details = {}, MessageKind kind = MessageKind::Diagnostic) const;

    /** Marks this context as failed in the progress node and synchronizes task progress. */
    void markFailed();

    /** Sets this context progress and synchronizes task progress. */
    void setProgress(double value) const;

    /** @return Current progress value for this context. */
    [[nodiscard]] double getProgress() const;

    /** @return Report node associated with this context. */
    [[nodiscard]] ReportNodePtr getReportNode() const;

    /** @return Progress node associated with this context. */
    [[nodiscard]] ProgressNodePtr getProgressNode() const;

    /** @return Shared execution state associated with this context. */
    [[nodiscard]] StatePtr getState() const;

    /** @return Progress aggregation mode for this context. */
    [[nodiscard]] WorkflowPlan::JobProgressMode getProgressMode() const;

    /**
     * @brief Returns the execution path.
     *
     * The path is built from this context and its ancestors, starting at the
     * root context.
     *
     * @param separator Separator inserted between path elements.
     * @return Execution path string.
     */
    [[nodiscard]] QString getExecutionPath(const QString& separator = "/") const;

    /**
     * @brief Returns the hierarchical depth.
     * @return Depth relative to the root context.
     */
    [[nodiscard]] std::int32_t getDepth() const;

    /** @return Workflow options from the shared execution state, or defaults if unavailable. */
    [[nodiscard]] WorkflowOptions getOptions() const;

public:

    /**
     * @brief Returns the unique context identifier.
     * @return Unique identifier of this workflow execution context.
     */
    [[nodiscard]] QUuid getId() const;

    /**
     * @brief Sets the output identifier used by setOutput() and takeOutput().
     * @param outputId Output identifier to route values through shared state.
     */
    void setOutputId(const QUuid& outputId);

    /** @return Output identifier used by this context. */
    [[nodiscard]] QUuid getOutputId() const;

    /**
     * @brief Returns the parent context identifier.
     * @return Parent context identifier, or a null QUuid for root contexts.
     */
    [[nodiscard]] QUuid getParentId() const;

public:

    /** @return Task associated with this context, or nullptr. */
    [[nodiscard]] Task* getTask() const;

public:

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
    [[nodiscard]] QVariant takeOutput(const WorkflowHandle& handle);

    /** @return Parent execution context, or nullptr for root contexts. */
    [[nodiscard]] SharedWorkflowExecutionContext getParent() const;

    /** @return Names of registered child contexts. */
    [[nodiscard]] QStringList getChildNames() const;

    /** @return True when the output id differs from this context id. */
    [[nodiscard]] bool hasExplicitOutputId() const;

private:

    /** Synchronizes the GUI task progress from the shared execution state. */
    void syncTaskProgress() const;

    /**
     * @brief Adds a lifecycle message to the report node and console output.
     * @param severity Message severity.
     * @param text Message text.
     * @param location Source or workflow location associated with the message.
     * @param details Additional structured details.
     */
    void addLifecycleMessage(util::SeverityLevel severity, QString text, QString location, QVariantMap details) const;

    /**
     * @brief Builds console output details for report messages.
     * @param details Additional details to include in the console output.
     * @return Variant map with context identity, hierarchy, and timing details for console output.
     */
    [[nodiscard]] QVariantMap makeConsoleDetails(const QVariantMap& details) const;

private:

    friend class WorkflowExecutionScope;

private:

    QString                                         _name;                                                      /**< Human-readable execution context name. */
    QUuid                                           _id;                                                        /**< Unique execution context identifier. */
    QUuid                                           _outputId;                                                  /**< Identifier used to route output values. */
    QUuid                                           _parentId;                                                  /**< Unique identifier of the parent context, if any. */
    QStringList                                     _executionPath;                                             /**< Hierarchical execution path from the root context. */
    ReportNodePtr                                   _reportNode;                                                /**< Report node associated with this context. */
    ProgressNodePtr                                 _progressNode;                                              /**< Progress node associated with this context. */
    StatePtr                                        _state;                                                     /**< Shared execution state for the root workflow. */
    QPointer<Task>                                  _task;                                                      /**< Task used for GUI progress reporting. */
    WorkflowPlan::JobProgressMode                   _progressMode = WorkflowPlan::JobProgressMode::Automatic;   /**< Progress aggregation mode. */
    Type                                            _type = Type::Workflow;                                     /**< Semantic execution node type. */
    std::weak_ptr<WorkflowExecutionContext>         _parent;                                                    /**< Parent execution context. */
    mutable QMutex                                  _childrenMutex;                                             /**< Protects child context lookup. */
    QHash<QString, SharedWorkflowExecutionContext>  _childrenByName;                                            /**< Child contexts indexed by name. */
};

/** Shared reference to a WorkflowExecutionContext. */
using SharedWorkflowExecutionContext = std::shared_ptr<WorkflowExecutionContext>;

}
