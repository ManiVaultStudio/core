// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Task.h"

namespace hdps {

/**
 * Dataset task class
 *
 * Task for reporting progress related to datasets (for instance in the data hierarchy plugin).
 *
 * @author Thomas Kroes
 */
class DatasetTask final : public Task
{
    Q_OBJECT

public:

    /**
    * Construct task with \p parent object, \p name and initial \p status
    * @param parent Pointer to parent object
    * @param name Name of the task
    * @param status Initial status of the task
    * @param mayKill Boolean determining whether the task may be killed or not
    */
    DatasetTask(QObject* parent, const QString& name, const Status& status = Status::Undefined, bool mayKill = false);
};

}