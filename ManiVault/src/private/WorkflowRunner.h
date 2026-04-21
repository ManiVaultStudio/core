// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include <QString>
#include <QObject>

class WorkflowRunner final : public QObject
{
    Q_OBJECT

public:
    using FinishedCallback = std::function<void(const mv::util::WorkflowResult&)>;
    using FailedCallback = std::function<void(const QString&)>;

    WorkflowRunner(mv::util::WorkflowPlan workflowPlan,
        mv::Task* task,
        FinishedCallback onFinished = {},
        FailedCallback onFailed = {},
        QObject* parent = nullptr);

public slots:
    void run();

signals:
    void progressChanged(double progress);
    void finished(const mv::util::WorkflowResult& result);
    void failed(const QString& error);

private:
    mv::util::WorkflowPlan _workflowPlan;
    mv::Task* _task = nullptr;
    FinishedCallback _onFinished;
    FailedCallback _onFailed;
};
