// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowReportNode.h"
#include "WorkflowProgressNode.h"

#include <QString>

namespace mv::util
{

class WorkflowExecutionContext
{
public:
    using ReportNodePtr = WorkflowReportNode::Ptr;
    using ProgressNodePtr = WorkflowProgressNode::Ptr;

    WorkflowExecutionContext() = default;

    WorkflowExecutionContext(const QString& name, const ReportNodePtr& reportNode, const ProgressNodePtr& progressNode) :
		_name(name),
		_reportNode(reportNode),
		_progressNode(progressNode)
    {
    }

    static WorkflowExecutionContext makeRoot(const QString& name)
    {
        auto reportRoot = std::make_shared<WorkflowReportNode>(name);
        auto progressRoot = std::make_shared<WorkflowProgressNode>(1.0);

        return WorkflowExecutionContext(name, reportRoot, progressRoot);
    }

    WorkflowExecutionContext createChild(const QString& name, double weight = 1.0) const
    {
        if (!_reportNode || !_progressNode)
            return {};

        return WorkflowExecutionContext(name, _reportNode->createChild(name), _progressNode->createChild(weight));
    }

    bool isValid() const
    {
        return static_cast<bool>(_reportNode) && static_cast<bool>(_progressNode);
    }

    QString getName() const
    {
        return _name;
    }

    void info(const QString& text, const QString& source = {}) const
    {
        if (_reportNode)
            _reportNode->addInfo(source, text);
    }

    void warning(const QString& text, const QString& source = {}) const
    {
        if (_reportNode)
            _reportNode->addWarning(source, text);
    }

    void error(const QString& text, const QString& source = {}) const
    {
        if (_reportNode)
            _reportNode->addError(source, text);
    }

    void setProgress(double value) const
    {
        if (_progressNode)
            _progressNode->setProgress(value);
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

    static const WorkflowExecutionContext* constCurrent()
    {
        return _current;
    }

    static WorkflowExecutionContext* current()
    {
        return _current;
    }

private:
    friend class WorkflowExecutionScope;

    static void setCurrent(WorkflowExecutionContext* ctx)
    {
        _current = ctx;
    }

private:
    QString         _name;
    ReportNodePtr   _reportNode;
    ProgressNodePtr _progressNode;

    static thread_local WorkflowExecutionContext* _current;
};

} // namespace mv::util
