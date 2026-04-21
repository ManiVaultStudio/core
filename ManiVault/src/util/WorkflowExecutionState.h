// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowExecutionNotifier.h"

#include <QString>
#include <QDateTime>

namespace mv::util
{

enum class WorkflowExecutionStatus {
    Idle,
    Running,
    Finished,
    Failed
};

class CORE_EXPORT WorkflowExecutionState
{
public:
    using Ptr = std::shared_ptr<WorkflowExecutionState>;

    WorkflowExecutionState(const WorkflowReportNode::Ptr& reportRoot,
        const WorkflowProgressNode::Ptr& progressRoot)
        : _reportRoot(reportRoot)
        , _progressRoot(progressRoot)
        , _notifier(new WorkflowExecutionNotifier())
    {
    }

    WorkflowReportNode::Ptr getReportRoot() const
    {
        return _reportRoot;
    }

    WorkflowProgressNode::Ptr getProgressRoot() const
    {
        return _progressRoot;
    }

    WorkflowExecutionNotifier* getNotifier() const
    {
        return _notifier.get();
    }

    WorkflowExecutionStatus getStatus() const
    {
        QMutexLocker lock(&_mutex);
        return _status;
    }

    void setStatus(WorkflowExecutionStatus status)
    {
        {
            QMutexLocker lock(&_mutex);
            _status = status;
        }

        emit _notifier->statusChanged();
    }

    double getOverallProgress() const
    {
        return _progressRoot ? _progressRoot->getProgress() : 0.0;
    }

    void notifyProgressChanged()
    {
        emit _notifier->progressChanged(getOverallProgress());
    }

    void notifyMessagesChanged()
    {
        emit _notifier->messagesChanged();
    }

    QVector<WorkflowMessage> collectMessages() const
    {
        QVector<WorkflowMessage> result;
        collectMessagesRecursive(_reportRoot, result);
        return result;
    }

private:
    static void collectMessagesRecursive(const WorkflowReportNode::Ptr& node,
        QVector<WorkflowMessage>& out)
    {
        if (!node)
            return;

        const auto messages = node->getMessages();
        for (const auto& message : messages)
            out.push_back(message);

        const auto children = node->getChildren();
        for (const auto& child : children)
            collectMessagesRecursive(child, out);
    }

private:
    WorkflowReportNode::Ptr _reportRoot;
    WorkflowProgressNode::Ptr _progressRoot;
    std::unique_ptr<WorkflowExecutionNotifier> _notifier;

    mutable QMutex _mutex;
    WorkflowExecutionStatus _status = WorkflowExecutionStatus::Idle;
};

} // namespace mv::util