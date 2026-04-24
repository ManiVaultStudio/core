// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowExecutionNotifier.h"
#include "WorkflowExecutionOptions.h"

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

    WorkflowExecutionState(const WorkflowReportNode::Ptr& reportRoot, const WorkflowProgressNode::Ptr& progressRoot, WorkflowExecutionOptions executionOptions = {});

    WorkflowReportNode::Ptr getReportRoot() const;

    WorkflowProgressNode::Ptr getProgressRoot() const;

    WorkflowExecutionOptions getExecutionOptions() const;

    WorkflowExecutionStatus getStatus() const;

    void setStatus(WorkflowExecutionStatus status);

    double getOverallProgress() const;

    QVector<WorkflowMessage> collectMessages() const;

private:
    static void collectMessagesRecursive(const WorkflowReportNode::Ptr& node,
        QVector<WorkflowMessage>& out);

private:
    WorkflowReportNode::Ptr _reportRoot;
    WorkflowProgressNode::Ptr _progressRoot;
    WorkflowExecutionOptions _executionOptions;

    mutable QMutex _mutex;
    WorkflowExecutionStatus _status = WorkflowExecutionStatus::Idle;
};

} // namespace mv::util