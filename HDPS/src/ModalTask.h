// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Task.h"

namespace hdps {

/**
 * Modal task class
 *
 * Model tasks are foreground tasks that show their progress in a modal progress dialog.
 *
 * @author Thomas Kroes
 */
class ModalTask final : public Task
{
    Q_OBJECT

public:

    /**
    * Construct progress with \p parent object, \p name and initial \p status
    * @param parent Pointer to parent object
    * @param name Name of the task
    * @param status Initial status of the task
    */
    ModalTask(QObject* parent, const QString& name, const Status& status = Status::Idle);
};

}
