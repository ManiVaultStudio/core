// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Task.h"

namespace hdps {

/**
 * Background task class
 *
 * Background tasks are shown in a main window status bar popup window.
 *
 * @author Thomas Kroes
 */
class BackgroundTask final : public Task
{
    Q_OBJECT

public:

    /**
    * Construct task with \p parent object, \p name and initial \p status
    * @param parent Pointer to parent object
    * @param name Name of the task
    * @param parentTask Pointer to parent task this will determine the placement in the task hierarchy as reflected in the user interface
    * @param status Initial status of the task
    * @param mayKill Boolean determining whether the task may be killed or not
    */
    BackgroundTask(QObject* parent, const QString& name, Task* parentTask = nullptr, const Status& status = Status::Undefined, bool mayKill = false);
};

}
