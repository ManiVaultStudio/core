// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowReportNode.h"
#include "WorkflowProgressNode.h"
#include "WorkflowExecutionState.h"
#include "Task.h"

#include <QString>

namespace mv::util
{

class CORE_EXPORT WorkflowExecutionContext
{
public:
    using ReportNodePtr = WorkflowReportNode::Ptr;
    using ProgressNodePtr = WorkflowProgressNode::Ptr;
    using StatePtr = WorkflowExecutionState::Ptr;

    WorkflowExecutionContext();

    WorkflowExecutionContext(QString name, ReportNodePtr reportNode, ProgressNodePtr progressNode, StatePtr state, Task* task = nullptr);

    static WorkflowExecutionContext makeRoot(const QString& name, Task* task, WorkflowExecutionOptions executionOptions = {});

    WorkflowExecutionContext createChild(const QString& name, double weight = 1.0) const;

    bool hasProgressChildren() const;

    bool isValid() const;

    QString getName() const;

    void info(const QString& text, const QString& source = {}) const;

    void warning(const QString& text, const QString& source = {}) const;

    void error(const QString& text, const QString& source = {}) const;

    void setProgress(double value) const;

    double getProgress() const;

    ReportNodePtr getReportNode() const;

    ProgressNodePtr getProgressNode() const;

    StatePtr getState() const;

    static WorkflowExecutionContext* current();

    static const WorkflowExecutionContext* currentConst();

private:
    friend class WorkflowExecutionScope;

    static void setCurrent(WorkflowExecutionContext* context);

private:
    QString _name;
    ReportNodePtr _reportNode;
    ProgressNodePtr _progressNode;
    StatePtr _state;
    QPointer<Task> _task;
};



} // namespace mv::util
