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

private:
    friend class WorkflowExecutionScope;

    static void setCurrent(WorkflowExecutionContext* context);

    

private:
    QString _name;
    ReportNodePtr _reportNode;
    ProgressNodePtr _progressNode;
    StatePtr _state;
    SharedThreadPool _threadPool;
    QPointer<Task> _task;
    WorkflowPlan::JobProgressMode _progressMode = WorkflowPlan::JobProgressMode::Automatic;
    std::vector<WorkflowResultFuture> _pendingAsyncWork;
};

} // namespace mv::util
