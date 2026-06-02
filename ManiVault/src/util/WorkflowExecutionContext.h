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

namespace mv::util
{

class CORE_EXPORT WorkflowExecutionContext : public std::enable_shared_from_this<WorkflowExecutionContext>
{
public:
    using ReportNodePtr = WorkflowReportNode::SharedWorkflowReportNode;
    using ProgressNodePtr = WorkflowProgressNode::Ptr;
    using StatePtr = WorkflowExecutionState::Ptr;
    using Type = WorkflowExecutionNodeType;

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

    static SharedWorkflowExecutionContext makeRoot(const QString& name, Task* task, WorkflowExecutionOptions executionOptions = {});

    SharedWorkflowExecutionContext createChild(Type type, const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic);

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
    SharedWorkflowExecutionContext createWorkflowChild(const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic);
    SharedWorkflowExecutionContext createNestedWorkflowChild(const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic);
    SharedWorkflowExecutionContext createSequentialStageChild(const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic);
    SharedWorkflowExecutionContext createParallelStageChild(const QString& name, double weight = 1.0,WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic); 
    SharedWorkflowExecutionContext createJobChild(const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic);
    SharedWorkflowExecutionContext createTypedChild(Type type, const QString& name, double weight,WorkflowPlan::JobProgressMode progressMode);

    bool isRootExecution() const;

public:

    void reportStarted() const;
    void reportFinished(std::uint64_t durationMs = 0) const;
    void reportFailed(SeverityLevel severity, const QString& errorMessage, QVariantMap extraDetails = {}) const;
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
     * @brief Retrieves and removes all result values associated with this workflow execution context as a QVariantMap. This is useful for cases where all result values should be accessed at once and then removed from the execution state to prevent further access or to free up resources.
     * @return A QVariantMap containing all result values associated with this workflow execution context, where the keys are the local keys used when publishing the result values and the values are the corresponding result values wrapped in QVariants. After this call, the execution state will no longer contain these result values.
     */
    [[nodiscard]] QVariantMap takeResultValues();

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
    Type                                    _type = Type::Workflow;                                     /** Semantic type of this workflow execution context, used for rendering, reporting, and diagnostics */
    std::weak_ptr<WorkflowExecutionContext> _parent;                                                    /** Weak pointer to the parent workflow execution context, if any */
};

/** Optional reference to a WorkflowExecutionContext */
using SharedWorkflowExecutionContext = std::shared_ptr<WorkflowExecutionContext>;

} // namespace mv::util
