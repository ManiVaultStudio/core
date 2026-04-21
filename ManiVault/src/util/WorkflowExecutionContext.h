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

    WorkflowExecutionContext() = default;

    WorkflowExecutionContext(const QString& name,
        const ReportNodePtr& reportNode,
        const ProgressNodePtr& progressNode,
        const StatePtr& state, Task* task = nullptr)
        : _name(name)
        , _reportNode(reportNode)
        , _progressNode(progressNode)
        , _state(state)
        , _task(task)
    {
    }

    static WorkflowExecutionContext makeRoot(const QString& name, Task* task = nullptr)
    {
        auto reportRoot = std::make_shared<WorkflowReportNode>(name);
        auto progressRoot = std::make_shared<WorkflowProgressNode>(1.0);
        auto state = std::make_shared<WorkflowExecutionState>(reportRoot, progressRoot);

        return WorkflowExecutionContext(name, reportRoot, progressRoot, state);
    }

    WorkflowExecutionContext createChild(const QString& name, double weight = 1.0) const
    {
        if (!_reportNode || !_progressNode || !_state)
            return {};

        return WorkflowExecutionContext(
            name,
            _reportNode->createChild(name),
            _progressNode->createChild(weight),
            _state,
            _task
        );
    }

    bool isValid() const
    {
        return static_cast<bool>(_reportNode)
            && static_cast<bool>(_progressNode)
            && static_cast<bool>(_state);
    }

    QString getName() const
    {
        return _name;
    }

    void info(const QString& text, const QString& source = {}) const
    {
        if (_reportNode)
            _reportNode->addMessage(WorkflowMessageLevel::Info, source, text);

        if (_state)
            _state->notifyMessagesChanged();

        //if (_task)
        //    _task->setStatusText(text);
    }

    void warning(const QString& text, const QString& source = {}) const
    {
        if (_reportNode)
            _reportNode->addMessage(WorkflowMessageLevel::Warning, source, text);

        if (_state)
            _state->notifyMessagesChanged();

        //if (_task)
        //    _task->addWarning(text);
    }

    void error(const QString& text, const QString& source = {}) const
    {
        if (_reportNode)
            _reportNode->addMessage(WorkflowMessageLevel::Error, source, text);

        if (_state)
            _state->notifyMessagesChanged();

        //if (_task)
        //    _task->addError(text);
    }

    void setProgress(double value) const
    {
        if (_progressNode)
            _progressNode->setProgress(value);

        if (_state)
            _state->notifyProgressChanged();

        if (_task && _state)
            _task->setProgress(_state->getOverallProgress());
    }

    double getProgress() const
    {
        return _progressNode ? _progressNode->getProgress() : 0.0;
    }

    ReportNodePtr getReportNode() const
    {
        return _reportNode;
    }

    ProgressNodePtr getProgressNode() const
    {
        return _progressNode;
    }

    StatePtr getState() const
    {
        return _state;
    }

    Task* getTask() const
    {
        return _task;
    }

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
    Task* _task = nullptr;
};



} // namespace mv::util
