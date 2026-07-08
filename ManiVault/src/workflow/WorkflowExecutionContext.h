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
 * @brief Represents the execution context for a workflow, stage, or job.
 *
 * A WorkflowExecutionContext encapsulates all state required while executing a workflow.
 * It provides access to progress reporting, execution reporting, logging, execution
 * options, task integration, hierarchical execution structure, and workflow outputs.
 *
 * Execution contexts form a tree that mirrors the logical structure of a workflow.
 * Child contexts inherit execution state from their parent while maintaining their
 * own identity, progress node, report node, and output scope.
 *
 * Responsibilities include:
 *
 * * Maintaining execution hierarchy (workflow, stages, jobs, nested workflows)
 * * Tracking progress through WorkflowProgressNode
 * * Recording execution reports through WorkflowReportNode
 * * Storing execution options and shared execution state
 * * Providing structured logging facilities
 * * Managing workflow outputs and child outputs
 * * Synchronizing progress with an associated Task
 * * Assigning unique identifiers for tracing and diagnostics
 *
 * Every workflow execution begins with a root context created using makeRoot().
 * Additional contexts are typically created through one of the create*Child()
 * helper functions, which automatically construct the appropriate hierarchy and
 * inherit shared execution state.
 *
 * The execution context is intentionally lightweight and shared using
 * std::shared_ptr. Parent references are stored as std::weak_ptr to avoid
 * ownership cycles while allowing traversal of the execution tree.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowExecutionContext : public std::enable_shared_from_this<WorkflowExecutionContext>
{
public:
    using ReportNodePtr = WorkflowReportNode::SharedWorkflowReportNode;
    using ProgressNodePtr = WorkflowProgressNode::Ptr;
    using StatePtr = WorkflowExecutionState::Ptr;
    using Type = WorkflowExecutionNodeType;

    /**
     * @brief Constructs a new WorkflowExecutionContext with default values.
     */
    WorkflowExecutionContext();

    /**
	 * @brief Constructs an execution context.
	 *
	 * @param name Human-readable context name.
	 * @param reportNode Associated report node.
	 * @param progressNode Associated progress node.
	 * @param state Shared execution state.
	 * @param task Optional task used for progress synchronization.
	 * @param progressMode Progress reporting mode.
	 */
    WorkflowExecutionContext(QString name, ReportNodePtr reportNode, ProgressNodePtr progressNode, StatePtr state, Task* task = nullptr, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic);

    /**
     * @brief Returns the semantic type of this execution context.
     *
     * @return The type of the execution context, indicating whether it represents a workflow, stage, job, or other execution unit.
     */
    [[nodiscard]] Type getType() const;

    /**
     * @brief Sets the semantic type of this execution context.
     * @param type New context type.
     */
    void setType(Type type);

    /**
     * @brief Creates the root execution context for a workflow execution.
     * @param name Human-readable root workflow name.
     * @param task Optional task used for progress synchronization.
     * @param executionOptions Options controlling workflow execution behavior.
     * @return Shared root execution context.
     */
    [[nodiscard]] static SharedWorkflowExecutionContext makeRoot(const QString& name, Task* task, WorkflowExecutionOptions executionOptions = {});

    /**
     * @brief Creates a child execution context of the specified type.
     *
     * This function creates a new workflow execution context that inherits execution state,
     * reporting infrastructure, progress tracking infrastructure, and task association from
     * the current context while establishing a new execution hierarchy level for a child context.
     *
     * The created child context:
     * - Shares the same workflow execution state
     * - Shares the same task association
     * - Extends the execution path hierarchy
     * - Creates child report and progress nodes
     * - Establishes a unique result scope
     * - Receives its own unique execution identifier
     *
     * @param type The semantic type of the child context (e.g., Workflow, Stage, Job).
     * @param name Human-readable name of the child context.
     * @param weight Relative progress contribution weight of the child context.
     * @param progressMode Progress reporting mode used by the child context.
     * @return Shared pointer to the newly created workflow execution context.
     */
    [[nodiscard]] SharedWorkflowExecutionContext createChild(Type type, const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic);

    /**
     * @brief Creates a child workflow execution context representing a top-level workflow scope.
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
    [[nodiscard]] SharedWorkflowExecutionContext createWorkflowChild(const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic);

    /**
     * @brief Creates a child context for a nested workflow.
     *
     * @param name Human-readable name of the nested workflow context.
     * @param weight Relative progress contribution weight of the nested workflow.
     * @param progressMode Progress reporting mode used by the nested workflow.
     * @return Shared pointer to the newly created workflow execution context.
     */
    [[nodiscard]] SharedWorkflowExecutionContext createNestedWorkflowChild(const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic);

    /**
     * @brief Creates a child context for a sequential stage.
     *
     * @param name Human-readable name of the sequential stage context.
     * @param weight Relative progress contribution weight of the sequential stage.
     * @param progressMode Progress reporting mode used by the sequential stage.
     * @return Shared pointer to the newly created workflow execution context.
     */
    [[nodiscard]] SharedWorkflowExecutionContext createSequentialStageChild(const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic);

    /**
     * @brief Creates a child context for a parallel stage.
     *
     * @param name Human-readable name of the parallel stage context.
     * @param weight Relative progress contribution weight of the parallel stage.
     * @param progressMode Progress reporting mode used by the parallel stage.
     * @return Shared pointer to the newly created workflow execution context.
     */
    [[nodiscard]] SharedWorkflowExecutionContext createParallelStageChild(const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic);

    /**
     * @brief Creates a child context for a job.
     *
     * @param name Human-readable name of the job context.
     * @param weight Relative progress contribution weight of the job.
     * @param progressMode Progress reporting mode used by the job.
     * @return Shared pointer to the newly created workflow execution context.
     */
    [[nodiscard]] SharedWorkflowExecutionContext createJobChild(const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic);

    /**
     * @brief Creates a typed child context.
     *
     * @param type The semantic type of the child context (e.g., Workflow, Stage, Job).
     * @param name Human-readable name of the child context.
     * @param weight Relative progress contribution weight of the child context.
     * @param progressMode Progress reporting mode used by the child context.
     * @return Shared pointer to the newly created workflow execution context.
     */
    [[nodiscard]] SharedWorkflowExecutionContext createTypedChild(Type type, const QString& name, double weight, WorkflowPlan::JobProgressMode progressMode);

    /**
     * Establish whether this execution context is the root of the workflow execution tree.
     * @return Boolean determining whether this context is the root execution context (true) or a child context (false).
     */
    [[nodiscard]] bool isRootExecution() const;

public: // Reporting

    /**
     * @brief Reports that the execution context has started.
     */
    void reportStarted() const;

    /**
     * @brief Reports that the execution context has finished.
     *
     * @param durationMs Optional duration of the execution in milliseconds. If provided, it will be included in the report details.
     */
    void reportFinished(std::uint64_t durationMs = 0);

    /**
     * @brief Reports that the execution context has failed.
     *
     * @param severity The severity level of the failure (e.g., Error, Fatal).
     * @param errorMessage A descriptive message explaining the reason for the failure.
     * @param extraDetails Optional additional details or metadata related to the failure, provided as a QVariantMap.
     */
    void reportFailed(util::SeverityLevel severity, const QString& errorMessage, QVariantMap extraDetails = {});

    /**
     * @brief Reports that the execution context has been skipped.
     *
     * @param reason A descriptive message explaining the reason for skipping the execution context.
     */
    void reportSkipped(const QString& reason);

    /**
     * @brief Reports a summary of the execution stage.
     *
     * @param summary A WorkflowStageSummary object containing the metrics and details of the execution stage.
     */
    void reportStageSummary(const WorkflowStageSummary& summary) const;

    /**
	 * @brief Creates structured lifecycle metadata.
	 *
	 * @param event Lifecycle event name.
	 * @param durationMs Optional execution duration.
	 * @return Lifecycle metadata.
	 */
    [[nodiscard]] QVariantMap makeLifecycleDetails(const QString& event, std::uint64_t durationMs = 0) const;

public:

    /**
     * @brief Checks if this execution context has any child contexts that are currently in progress.
     *
     * @return True if there are child contexts that are still in progress; false otherwise.
     */
    [[nodiscard]] bool hasProgressChildren() const;

    /**
     * @brief Checks if this execution context is valid and properly initialized.
     *
     * @return True if the execution context is valid; false otherwise.
     */
    [[nodiscard]] bool isValid() const;

    /**
	 * @brief Returns the human-readable name of this execution context.
	 *
	 * @return The execution context name.
	 */
    [[nodiscard]] QString getName() const;

    /**
     * @brief Adds a message to the report node associated with this workflow execution context.
     *
     * @param severity The severity level of the message (e.g., Info, Warning, Error).
     * @param text The main text or content of the message to be added to the report node.
     * @param location The specific location in the code or workflow where the message was generated (e.g., function name, line number). This can be used to provide additional context for the message and help with debugging and tracing.
     * @param details Additional details or metadata associated with the message (optional). This can be used to provide further context or structured information related to the message, such as variable values, execution state, or other relevant data that may assist in understanding the message and its implications within the workflow execution.
     */
    void message(util::SeverityLevel severity, QString text, QString location, QVariantMap details) const;

    /**
     * @brief Adds an informational message to the report node associated with this workflow execution context.
     *
     * @param text The main text or content of the informational message to be added to the report node.
     * @param location The specific location in the code or workflow where the informational message was generated (e.g., function name, line number). This can be used to provide additional context for the message and help with debugging and tracing.
     * @param details Additional details or metadata associated with the informational message (optional). This can be used to provide further context or structured information related to the message, such as variable values, execution state, or other relevant data that may assist in understanding the message and its implications within the workflow execution.
     */
    void info(QString text, QString location = {}, QVariantMap details = {}) const;

    /**
     * @brief Adds a warning message to the report node associated with this workflow execution context.
     *
     * @param text The main text or content of the warning message to be added to the report node.
     * @param location The specific location in the code or workflow where the warning message was generated (e.g., function name, line number). This can be used to provide additional context for the message and help with debugging and tracing.
     * @param details Additional details or metadata associated with the warning message (optional). This can be used to provide further context or structured information related to the message, such as variable values, execution state, or other relevant data that may assist in understanding the message and its implications within the workflow execution.
     */
    void warning(QString text, QString location = {}, QVariantMap details = {}) const;

    /**
     * @brief Adds an error message to the report node associated with this workflow execution context.
     *
     * @param text The main text or content of the error message to be added to the report node.
     * @param location The specific location in the code or workflow where the error message was generated (e.g., function name, line number). This can be used to provide additional context for the message and help with debugging and tracing.
     * @param details Additional details or metadata associated with the error message (optional). This can be used to provide further context or structured information related to the message, such as variable values, execution state, or other relevant data that may assist in understanding the message and its implications within the workflow execution.
     */
    void error(QString text, QString location = {}, QVariantMap details = {}) const;

    /**
     * @brief Updates the progress value associated with this context.
     * @param value Progress value, in the range [0.0, 1.0].
     */
    void setProgress(double value) const;

    /**
     * @brief Returns the current progress value associated with this context.
     * @return The current progress value, in the range [0.0, 1.0].
     */
    [[nodiscard]] double getProgress() const;

    /**
     * @brief Returns the report node associated with this context.
     *
     * @return Shared pointer to the report node.
     */
    [[nodiscard]] ReportNodePtr getReportNode() const;

    /**
     * @brief Returns the progress node associated with this context.
     *
     * @return Shared pointer to the progress node.
     */
    [[nodiscard]] ProgressNodePtr getProgressNode() const;

    /**
     * @brief Returns the execution state associated with this context.
     *
     * @return Shared pointer to the execution state.
     */
    [[nodiscard]] StatePtr getState() const;

    /**
     * @brief Returns the progress reporting mode associated with this context.
     *
     * @return The progress reporting mode for this context.
     */
    [[nodiscard]] WorkflowPlan::JobProgressMode getProgressMode() const;

    /**
     * @brief Gets the execution path of this workflow execution context as a string, with each level of the hierarchy separated by the specified separator. The execution path is constructed by concatenating the names of this context and all of its ancestor contexts, starting from the root context down to this context. This can be useful for logging and tracing purposes to provide a clear and human-readable representation of where in the workflow hierarchy a particular event or message is occurring.
     * @param separator The string to use as a separator between levels of the execution path. The default value is " / ", but it can be customized to use any other separator as needed (e.g., " > ", " -> ", etc.).
     * @return The execution path of this workflow execution context as a string, with each level of the hierarchy separated by the specified separator.
     */
    [[nodiscard]] QString getExecutionPath(const QString& separator = "/") const;

    /**
     * @brief Gets the hierarchical depth of this workflow execution context within the workflow execution tree.
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
    [[nodiscard]] std::int32_t getDepth() const;

    /**
	 * @brief Returns the execution options associated with this context.
	 *
	 * Child contexts inherit these options from the root execution context.
	 *
	 * @return The workflow execution options.
	 */
    [[nodiscard]] WorkflowExecutionOptions getExecutionOptions() const;

public: // ID

    /**
     * @brief Get the unique identifier of this workflow execution context. This ID is used for tracing and logging purposes, and can be used to correlate log messages and trace events with specific workflow executions.
     * @return The unique identifier of this workflow execution context.
     */
    [[nodiscard]] QUuid getId() const;

    /**
     * @brief Sets the output identifier used by this context.
     *
     * @param outputId Identifier used to publish or retrieve this context's output.
     */
    void setOutputId(const QUuid& outputId);

    /**
     * @brief Returns the output identifier associated with this context.
     *
     * @return The output identifier used to publish or retrieve this context's output.
     */
    [[nodiscard]] QUuid getOutputId() const;

    /**
     * Get the unique identifier of the parent workflow execution context, if any. This can be used to correlate log messages and trace events with parent-child relationships between workflow executions.
     * @return The unique identifier of the parent workflow execution context, or a null QUuid if this is a root context.
     */
    [[nodiscard]] QUuid getParentId() const;

public: // Task management

    /**
     * @brief Returns the task associated with this context, if any.
     *
     * @return Pointer to the associated Task, or nullptr if no task is associated.
     */
    [[nodiscard]] Task* getTask() const;

public: // Child context and output  management

    /**
     * @brief Registers a named child context.
     *
     * @param name Name under which the child context should be stored.
     * @param child Child context to register.
     */
    void registerChildContext(const QString& name, const SharedWorkflowExecutionContext& child);

    /**
     * @brief Returns a registered child context by name.
     *
     * @param name Name of the child context.
     * @return Shared child context, or nullptr if no child with this name exists.
     */
    [[nodiscard]] SharedWorkflowExecutionContext getChildContext(const QString& name) const;

    /**
     * @brief Stores the output value produced by this context.
     *
     * @param value Output value to store.
     */
    void setOutput(const QVariant& value);

    /**
    * @brief Takes and removes the output value produced by this context.
    *
    * @return Stored output value.
    */
    [[nodiscard]] QVariant takeOutput();

    /**
     * @brief Takes and removes the output value associated with a workflow handle.
     *
     * @param handle Workflow handle identifying the output-producing context.
     * @return Stored output value.
     */
    [[nodiscard]] QVariant takeOutput(const WorkflowHandle& handle);

    /**
     * @brief Returns the parent execution context.
     *
     * @return Shared pointer to the parent workflow execution context, or nullptr if this is a root context.
     */
    [[nodiscard]] SharedWorkflowExecutionContext getParent() const;

    /**
     * @brief Returns the names of all registered child contexts.
     *
     * @return A QStringList containing the names of all registered child contexts.
     */
    [[nodiscard]] QStringList getChildNames() const;

    /**
     * @brief Returns whether this context has an explicitly assigned output identifier.
     *
     * @return True if an explicit output identifier has been set; false otherwise.
     */
    [[nodiscard]] bool hasExplicitOutputId() const;

private:

    /**
     * @brief Synchronizes the progress of this execution context with the associated task, if any.
     *
     * This function updates the progress of the associated task based on the current progress value of this execution context.
     * It is typically called after progress updates to ensure that the task's progress reflects the current state of the workflow execution.
     */
    void syncTaskProgress() const;

private:
    friend class WorkflowExecutionScope;

private:
    QString                                         _name;                                                      /** Name of the workflow execution context, typically derived from the name of the workflow plan or job it represents */
    QUuid                                           _id;                                                        /** Unique identifier for this workflow execution context */
    QUuid                                           _outputId;                                                  /** Unique identifier for the output of this workflow execution context */  
    QUuid                                           _parentId;                                                  /** Unique identifier of the parent workflow execution context, if any */
    QStringList                                     _executionPath;                                             /** Execution path of this workflow execution context */
    ReportNodePtr                                   _reportNode;                                                /** Report node associated with this workflow execution context */
    ProgressNodePtr                                 _progressNode;                                              /** Progress node associated with this workflow execution context */
    StatePtr                                        _state;                                                     /** Execution state associated with this workflow execution context */
    QPointer<Task>                                  _task;                                                      /** Task associated with this workflow execution context */
    WorkflowPlan::JobProgressMode                   _progressMode = WorkflowPlan::JobProgressMode::Automatic;   /** Progress mode for this workflow execution context */
    Type                                            _type = Type::Workflow;                                     /** Semantic type of this workflow execution context, used for rendering, reporting, and diagnostics */
    std::weak_ptr<WorkflowExecutionContext>         _parent;                                                    /** Weak pointer to the parent workflow execution context, if any */
    mutable QMutex                                  _childrenMutex;                                             /** Mutex for synchronizing access to child contexts */
    QHash<QString, SharedWorkflowExecutionContext>  _childrenByName;                                            /** Hash map of child contexts indexed by name */
};

/** Optional reference to a WorkflowExecutionContext */
using SharedWorkflowExecutionContext = std::shared_ptr<WorkflowExecutionContext>;

}
