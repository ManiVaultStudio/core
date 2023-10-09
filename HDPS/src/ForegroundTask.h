// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Task.h"

namespace hdps {

class ForegroundTaskHandler;

/**
 * Foreground task class
 *
 * Foreground tasks appear in a main window status bar popup window.
 *
 * @author Thomas Kroes
 */
class ForegroundTask final : public Task
{
    Q_OBJECT

public:

    /**
    * Construct task with \p parent object, \p name and some optional parameters
    * @param parent Pointer to parent object
    * @param name Name of the task
    * @param status Initial status of the task
    * @param mayKill Boolean determining whether the task may be killed or not
    */
    ForegroundTask(QObject* parent, const QString& name, const Status& status = Status::Undefined, bool mayKill = false);

    /**
     * Creates singleton foreground task handler
     * @param parent Pointer to parent object
     */
    static void createHandler(QObject* parent);

    /**
     * Get global handler
     * @return Pointer to foreground task handler
     */
    static ForegroundTaskHandler* getGlobalHandler() { return ForegroundTask::foregroundTaskHandler; }

private:
    static ForegroundTaskHandler* foregroundTaskHandler;   /** Single instance of the foreground task handler (popup action in the main window status bar) */
};

}
