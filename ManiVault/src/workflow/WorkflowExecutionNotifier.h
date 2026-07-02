// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "Task.h"

namespace mv::workflow
{

/**
 * @brief Emits Qt notifications for workflow execution state changes.
 *
 * WorkflowExecutionNotifier is a QObject bridge used by workflow execution code
 * to publish progress, message, and status updates to UI or task-related
 * consumers. An optional Task can be associated so emitted progress updates can
 * be reflected in task-driven UI.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowExecutionNotifier : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Constructs a workflow execution notifier.
     * @param parent Optional QObject parent.
     */
    explicit WorkflowExecutionNotifier(QObject* parent = nullptr);

    /**
     * @brief Associates a task with this notifier.
     * @param task Task that should receive workflow progress updates.
     */
    void setTask(Task* task);

signals:

    /**
     * @brief Emitted when overall workflow progress changes.
     * @param overallProgress Normalized workflow progress in the range [0.0, 1.0].
     */
    void progressChanged(double overallProgress);

    /**
     * @brief Emitted when workflow messages change.
     */
    void messagesChanged();

    /**
     * @brief Emitted when workflow execution status changes.
     */
    void statusChanged();
};

}
