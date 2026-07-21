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
 */
class CORE_EXPORT WorkflowExecutionNotifier : public QObject
{
    Q_OBJECT

public:

    /** Constructs a workflow execution notifier. */
    explicit WorkflowExecutionNotifier(QObject* parent = nullptr);

    /** Sets the task whose progress is observed. */
    void setTask(Task* task);

signals:

    /** Emitted when the overall workflow progress changes. */
    void progressChanged(double overallProgress);

    /** Emitted when workflow messages change. */
    void messagesChanged();

    /** Emitted when workflow execution status changes. */
    void statusChanged();
};

}
