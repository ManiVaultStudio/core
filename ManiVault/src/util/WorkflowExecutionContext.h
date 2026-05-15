// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowReportNode.h"
#include "WorkflowProgressNode.h"
#include "WorkflowExecutionState.h"
#include "WorkflowPlan.h"
#include "Task.h"

#include <QString>
#include <QThreadPool>
#include <QUuid>

namespace mv::util
{

class CORE_EXPORT WorkflowExecutionContext
{
protected:

    struct PendingAsyncWork
    {
        WorkflowResultFuture _future;
        QString _label;
    };

public:
    using ReportNodePtr = WorkflowReportNode::SharedWorkflowReportNode;
    using ProgressNodePtr = WorkflowProgressNode::Ptr;
    using StatePtr = WorkflowExecutionState::Ptr;
    using SharedThreadPool = std::shared_ptr<QThreadPool>;

    WorkflowExecutionContext();

    WorkflowExecutionContext(QString name, ReportNodePtr reportNode, ProgressNodePtr progressNode, StatePtr state, SharedThreadPool threadPool, Task* task = nullptr, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic);

    static SharedWorkflowExecutionContext makeRoot(const QString& name, Task* task, WorkflowExecutionOptions executionOptions = {});

    SharedWorkflowExecutionContext createChild(const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic) const;

    bool hasProgressChildren() const;

    bool isValid() const;

    QString getName() const;

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

    QThreadPool& getThreadPool();

    static SharedWorkflowExecutionContext current();

    static const SharedWorkflowExecutionContext currentConst();

    WorkflowPlan::JobProgressMode getProgressMode() const;

    void addPendingAsyncWork(WorkflowResultFuture future, const QString& label = {});

    void waitForPendingAsyncWork();

    /**
     * @brief Gets the execution path of this workflow execution context as a string, with each level of the hierarchy separated by the specified separator. The execution path is constructed by concatenating the names of this context and all of its ancestor contexts, starting from the root context down to this context. This can be useful for logging and tracing purposes to provide a clear and human-readable representation of where in the workflow hierarchy a particular event or message is occurring.
     * @param separator The string to use as a separator between levels of the execution path. The default value is " / ", but it can be customized to use any other separator as needed (e.g., " > ", " -> ", etc.).
     * @return The execution path of this workflow execution context as a string, with each level of the hierarchy separated by the specified separator.
     */
    QString getExecutionPath(const QString& separator = " / ") const;

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

    /**
     * Get the number of pending asynchronous work items (futures) that have been added to this workflow execution context and have not yet completed. This can be used to determine if there are any outstanding asynchronous operations that need to complete before the workflow execution can be considered finished.
     * @return The number of pending asynchronous work items.
     */
    std::size_t getPendingAsyncWorkCount() const;

private:
    friend class WorkflowExecutionScope;

    static void setCurrent(SharedWorkflowExecutionContext context);

private:
    QString                         _name;                                                      /** Name of the workflow execution context, typically derived from the name of the workflow plan or job it represents */
    QUuid                           _id;                                                        /** Unique identifier for this workflow execution context */
    QUuid                           _parentId;                                                  /** Unique identifier of the parent workflow execution context, if any */
    QStringList                     _executionPath;                                             /** Execution path of this workflow execution context */
    ReportNodePtr                   _reportNode;                                                /** Report node associated with this workflow execution context */
    ProgressNodePtr                 _progressNode;                                              /** Progress node associated with this workflow execution context */
    StatePtr                        _state;                                                     /** Execution state associated with this workflow execution context */
    SharedThreadPool                _threadPool;                                                /** Thread pool used for executing tasks within this workflow execution context */
    QPointer<Task>                  _task;                                                      /** Task associated with this workflow execution context */
    WorkflowPlan::JobProgressMode   _progressMode = WorkflowPlan::JobProgressMode::Automatic;   /** Progress mode for this workflow execution context */  
    std::vector<PendingAsyncWork>   _pendingAsyncWork;                                          /** List of pending asynchronous work (futures) that need to be completed before this workflow execution context can be considered finished */
};

/** Optional reference to a WorkflowExecutionContext */
using SharedWorkflowExecutionContext = std::shared_ptr<WorkflowExecutionContext>;

} // namespace mv::util
