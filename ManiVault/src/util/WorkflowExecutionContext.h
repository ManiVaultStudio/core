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
#include "Task.h"

#include <QString>
#include <QUuid>

namespace mv::util
{

class CORE_EXPORT WorkflowExecutionContext
{
public:

    /**
	 * @brief Describes the semantic role of a workflow execution context within the workflow execution hierarchy.
	 *
	 * Workflow execution contexts are used to represent different levels and types of execution entities
	 * within a workflow plan, such as entire workflows, stages, and individual jobs. The context type
	 * determines how the execution context should be interpreted, visualized, and reported in logging,
	 * progress reporting, tracing, and workflow diagnostics.
	 *
	 * This classification is primarily useful for:
	 * - Rendering hierarchical console and debug output
	 * - Distinguishing workflows from stages and jobs in reports
	 * - Visualizing workflow execution trees
	 * - Applying different progress aggregation strategies
	 * - Improving workflow tracing and diagnostics
	 * - Generating structured execution telemetry
	 *
	 * The different context types are:
	 *
	 * - Workflow:
	 *   Represents a top-level workflow execution context.
	 *
	 * - NestedWorkflow:
	 *   Represents a workflow executed as part of another workflow.
	 *
	 * - SequentialStage:
	 *   Represents a stage whose jobs or child stages execute sequentially.
	 *
	 * - ParallelStage:
	 *   Represents a stage whose jobs or child stages may execute concurrently.
	 *
	 * - Job:
	 *   Represents an individual executable unit of work within a workflow stage.
	 */
    enum class Type
    {
        Workflow,          /** Top-level workflow execution context */
        NestedWorkflow,    /** Workflow executed as part of another workflow */
        SequentialStage,   /** Sequential execution stage */
        ParallelStage,     /** Parallel execution stage */
        Job                /** Individual executable job */
    };

public:
    using ReportNodePtr = WorkflowReportNode::SharedWorkflowReportNode;
    using ProgressNodePtr = WorkflowProgressNode::Ptr;
    using StatePtr = WorkflowExecutionState::Ptr;

    WorkflowExecutionContext();

    WorkflowExecutionContext(QString name, ReportNodePtr reportNode, ProgressNodePtr progressNode, StatePtr state, Task* task = nullptr, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic);

    
    Type getType() const
    {
        return _type;
    }

    void setType(Type type)
    {
        _type = type;
    }

    static QString getWorkflowExecutionContextTypeName(Type type);

    static SharedWorkflowExecutionContext makeRoot(const QString& name, Task* task, WorkflowExecutionOptions executionOptions = {});

    SharedWorkflowExecutionContext createChild(const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic) const;

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
    SharedWorkflowExecutionContext createWorkflowChild(const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic) const;
    SharedWorkflowExecutionContext createNestedWorkflowChild(const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic) const;
    SharedWorkflowExecutionContext createSequentialStageChild(const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic) const;
    SharedWorkflowExecutionContext createParallelStageChild(const QString& name, double weight = 1.0,WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic) const; 
    SharedWorkflowExecutionContext createJobChild(const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic) const;
    SharedWorkflowExecutionContext createTypedChild(const QString& name, Type type, double weight,WorkflowPlan::JobProgressMode progressMode) const;

public:

    void reportStarted() const;
    void reportFinished(std::uint64_t durationMs = 0) const;
    void reportFailed(const QString& errorMessage) const;
    void reportSkipped(const QString& reason) const;
    void reportStageSummary(const WorkflowStageSummary& summary) const;

    QVariantMap makeLifecycleDetails(const QString& event, std::uint64_t durationMs = 0) const;

public:

    bool hasProgressChildren() const;

    bool isValid() const;

    QString getName() const;

    void message(SeverityLevel severity, QString text, QString location, QVariantMap details) const;

    /**
     * @brief Adds an informational message to the report node associated with this workflow execution context.
     * @param text The main text or content of the informational message to be added to the report node.
     * @param location The specific location in the code or workflow where the informational message was generated (e.g., function name, line number). This can be used to provide additional context for the message and help with debugging and tracing.
     * @param details Additional details or metadata associated with the informational message (optional). This can be used to provide further context or structured information related to the message, such as variable values, execution state, or other relevant data that may assist in understanding the message and its implications within the workflow execution.
     */
    void info(QString text, QString location = {}, QVariantMap details = {}) const;

    /**
     * @brief Adds a warning message to the report node associated with this workflow execution context.
     * @param text The main text or content of the warning message to be added to the report node.
     * @param location The specific location in the code or workflow where the warning message was generated (e.g., function name, line number). This can be used to provide additional context for the message and help with debugging and tracing.
     * @param details Additional details or metadata associated with the warning message (optional). This can be used to provide further context or structured information related to the message, such as variable values, execution state, or other relevant data that may assist in understanding the message and its implications within the workflow execution.
     */
    void warning(QString text, QString location = {}, QVariantMap details = {}) const;

    /**
     * @brief Adds an error message to the report node associated with this workflow execution context.
     * @param text The main text or content of the error message to be added to the report node.
     * @param location The specific location in the code or workflow where the error message was generated (e.g., function name, line number). This can be used to provide additional context for the message and help with debugging and tracing.
     * @param details Additional details or metadata associated with the error message (optional). This can be used to provide further context or structured information related to the message, such as variable values, execution state, or other relevant data that may assist in understanding the message and its implications within the workflow execution.
     */
    void error(QString text, QString location = {}, QVariantMap details = {}) const;

    void setProgress(double value) const;

    double getProgress() const;

    ReportNodePtr getReportNode() const;

    ProgressNodePtr getProgressNode() const;

    StatePtr getState() const;

    WorkflowPlan::JobProgressMode getProgressMode() const;

    /**
     * @brief Gets the execution path of this workflow execution context as a string, with each level of the hierarchy separated by the specified separator. The execution path is constructed by concatenating the names of this context and all of its ancestor contexts, starting from the root context down to this context. This can be useful for logging and tracing purposes to provide a clear and human-readable representation of where in the workflow hierarchy a particular event or message is occurring.
     * @param separator The string to use as a separator between levels of the execution path. The default value is " / ", but it can be customized to use any other separator as needed (e.g., " > ", " -> ", etc.).
     * @return The execution path of this workflow execution context as a string, with each level of the hierarchy separated by the specified separator.
     */
    QString getExecutionPath(const QString& separator = "/") const;

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
    std::int32_t getDepth() const;

public: // ID

    /**
     * Get the unique identifier of this workflow execution context. This ID is used for tracing and logging purposes, and can be used to correlate log messages and trace events with specific workflow executions.
     * @return The unique identifier of this workflow execution context.
     */
    QUuid getId() const;

    /**
     * Get the unique identifier of the parent workflow execution context, if any. This can be used to correlate log messages and trace events with parent-child relationships between workflow executions.
     * @return The unique identifier of the parent workflow execution context, or a null QUuid if this is a root context.
     */
    QUuid getParentId() const;

public: // Result values

    /**
     * @brief Publishes a result value associated with this workflow execution context. The result value is stored in the execution state and can be retrieved later using the specified key. This allows different parts of the workflow to share data and results in a structured way, enabling better communication and coordination between different steps and components of the workflow.
     * @tparam T The type of the value being published. This can be any type that can be stored in a QVariant, such as basic types (e.g., int, double, QString) or custom types that have been registered with the Qt meta-object system.
     * @param localKey The key associated with the result value, used to identify and access the value later. This key should be unique within the context of this workflow execution to avoid conflicts with other result values. The actual key used for storage may be scoped or namespaced based on the hierarchy of workflow execution contexts to ensure uniqueness across the entire workflow execution.
     * @param value The value to be published and stored in the execution state. This can be any value that can be converted to a QVariant, and will be stored in a way that allows it to be retrieved later using the specified key.
     */
    template<typename T>
    void publishResultValue(const QString& localKey, T&& value) {
        _state->setResultValue(localKey, QVariant::fromValue(std::forward<T>(value)));
    }

    /**
     * @brief Retrieves the result value associated with the specified key from the execution state of this workflow execution context. This allows different parts of the workflow to access shared data and results that have been published by other steps or components of the workflow, enabling better communication and coordination between different parts of the workflow.
     * @param localKey The key associated with the result value to be retrieved. This should match the key that was used when the result value was published using publishResultValue(). The actual key used for retrieval may be scoped or namespaced based on the hierarchy of workflow execution contexts, so it should be consistent with how the key was defined when publishing the result value.
     * @return The result value associated with the specified key, wrapped in a QVariant. If no value is associated with the key, this may return an invalid QVariant or throw an exception, depending on the implementation of WorkflowExecutionState.
     */
    [[nodiscard]] QVariant getResultValue(const QString& localKey) const;

    /**
     * @brief Retrieves and removes the result value associated with the specified key from the execution state of this workflow execution context. This is useful for cases where a result value should only be accessed once, and should be removed from the execution state after being retrieved to prevent it from being accessed again or to free up resources.
     * @param localKey The key associated with the result value to be retrieved and removed. This should match the key that was used when the result value was published using publishResultValue().
     * @return The result value associated with the specified key, wrapped in a QVariant. If no value is associated with the key, this may return an invalid QVariant or throw an exception, depending on the implementation of WorkflowExecutionState.
     */
    [[nodiscard]] QVariant takeResultValue(const QString& localKey) const;

    /**
     * @brief Checks if a result value with the specified key exists in the execution state of this workflow execution context. This can be used to determine whether a particular piece of information has been published and is available for retrieval before attempting to access it, allowing for safer and more robust code when working with dynamic result reporting.
     * @param localKey The key to check for existence in the execution state. This should match the key that was used when the result value was published using publishResultValue().
     * @return True if a result value with the specified key exists in the execution state, false otherwise.
     */
    [[nodiscard]] bool hasResultValue(const QString& localKey) const;

private: // Result value

    QString scopedResultKey(const QString& localKey) const;

private:
    friend class WorkflowExecutionScope;

private:
    QString                                 _name;                                                      /** Name of the workflow execution context, typically derived from the name of the workflow plan or job it represents */
    QUuid                                   _id;                                                        /** Unique identifier for this workflow execution context */
    QUuid                                   _parentId;                                                  /** Unique identifier of the parent workflow execution context, if any */
    QStringList                             _executionPath;                                             /** Execution path of this workflow execution context */
    ReportNodePtr                           _reportNode;                                                /** Report node associated with this workflow execution context */
    ProgressNodePtr                         _progressNode;                                              /** Progress node associated with this workflow execution context */
    StatePtr                                _state;                                                     /** Execution state associated with this workflow execution context */
    QPointer<Task>                          _task;                                                      /** Task associated with this workflow execution context */
    WorkflowPlan::JobProgressMode           _progressMode = WorkflowPlan::JobProgressMode::Automatic;   /** Progress mode for this workflow execution context */
    QString                                 _resultScope;                                               /** Scope for result values published by this workflow execution context, used to ensure uniqueness of result keys across the workflow execution hierarchy */
    Type                                    _type = Type::Workflow;                                     /** Semantic type of this workflow execution context, used for rendering, reporting, and diagnostics */
    std::weak_ptr<WorkflowExecutionContext> _parent;                                                    /** Weak pointer to the parent workflow execution context, if any */  
};

/** Optional reference to a WorkflowExecutionContext */
using SharedWorkflowExecutionContext = std::shared_ptr<WorkflowExecutionContext>;

} // namespace mv::util
