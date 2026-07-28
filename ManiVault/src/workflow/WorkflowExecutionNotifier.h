// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "Task.h"

namespace mv::workflow
{

/**
 * @brief QObject notifier for workflow execution UI updates.
 *
 * WorkflowExecutionNotifier exposes workflow progress and status changes as Qt
 * signals so UI components can observe execution without coupling to the
 * executor internals.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowExecutionNotifier : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Constructs a workflow execution notifier.
     * @param parent Optional parent object.
     */
    explicit WorkflowExecutionNotifier(QObject* parent = nullptr);

    /**
     * @brief Sets the task whose progress is observed.
     * @param task Task to observe, or nullptr.
     */
    void setTask(Task* task);

signals:

    /**
     * @brief Emitted when the overall workflow progress changes.
     * @param overallProgress Normalized workflow progress.
     */
    void progressChanged(double overallProgress);

    /** Emitted when workflow messages change. */
    void messagesChanged();

    /** Emitted when workflow execution status changes. */
    void statusChanged();
};

}
