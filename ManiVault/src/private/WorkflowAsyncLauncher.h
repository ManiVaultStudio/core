// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include <QObject>
#include <QString>

class WorkflowAsyncLauncher final : public QObject
{
    Q_OBJECT

public:
    explicit WorkflowAsyncLauncher(QObject* parent = nullptr);

    using FinishedCallback = std::function<void(const mv::util::WorkflowResult&)>;
    using FailedCallback = std::function<void(const QString&)>;

    static void startWorkflowAsync(mv::util::WorkflowPlan workflowPlan, mv::Task* task, QObject* contextObject, FinishedCallback onFinished = {}, FailedCallback onFailed = {});
};
