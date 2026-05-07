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
public:
    using ReportNodePtr = WorkflowReportNode::Ptr;
    using ProgressNodePtr = WorkflowProgressNode::Ptr;
    using StatePtr = WorkflowExecutionState::Ptr;
    using SharedThreadPool = std::shared_ptr<QThreadPool>;

    WorkflowExecutionContext();

    WorkflowExecutionContext(QString name, ReportNodePtr reportNode, ProgressNodePtr progressNode, StatePtr state, SharedThreadPool threadPool, Task* task = nullptr, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic);

    static WorkflowExecutionContext makeRoot(const QString& name, Task* task, WorkflowExecutionOptions executionOptions = {});

    WorkflowExecutionContext createChild(const QString& name, double weight = 1.0, WorkflowPlan::JobProgressMode progressMode = WorkflowPlan::JobProgressMode::Automatic) const;

    bool hasProgressChildren() const;

    bool isValid() const;

    QString getName() const;

    void info(const QString& text, const QString& source = {}, const QString& scope = {}, QVariantMap details = {}) const;

    void warning(const QString& text, const QString& source = {}, const QString& scope = {}, QVariantMap details = {}) const;

    void error(const QString& text, const QString& source = {}, const QString& scope = {}, QVariantMap details = {}) const;

    void setProgress(double value) const;

    double getProgress() const;

    ReportNodePtr getReportNode() const;

    ProgressNodePtr getProgressNode() const;

    StatePtr getState() const;

    QThreadPool& getThreadPool();

    static WorkflowExecutionContext* current();

    static const WorkflowExecutionContext* currentConst();

    WorkflowPlan::JobProgressMode getProgressMode() const;

    void addPendingAsyncWork(WorkflowResultFuture future);

    void waitForPendingAsyncWork();

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

private:
    friend class WorkflowExecutionScope;

    static void setCurrent(WorkflowExecutionContext* context);

    

private:
    QString                         _name;                                                      /** Name of the workflow execution context, typically derived from the name of the workflow plan or job it represents */
    QUuid                           _id;                                                        /** Unique identifier for this workflow execution context */
    QUuid                           _parentId;                                                  /** Unique identifier of the parent workflow execution context, if any */
    ReportNodePtr                   _reportNode;                                                /** Report node associated with this workflow execution context */
    ProgressNodePtr                 _progressNode;                                              /** Progress node associated with this workflow execution context */
    StatePtr                        _state;                                                     /** Execution state associated with this workflow execution context */
    SharedThreadPool                _threadPool;                                                /** Thread pool used for executing tasks within this workflow execution context */
    QPointer<Task>                  _task;                                                      /** Task associated with this workflow execution context */
    WorkflowPlan::JobProgressMode   _progressMode = WorkflowPlan::JobProgressMode::Automatic;   /** Progress mode for this workflow execution context */  
    WorkflowResultFutures           _pendingAsyncWork;                                          /** List of pending asynchronous work (futures) that need to be completed before this workflow execution context can be considered finished */
};

} // namespace mv::util
