// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ModalTask.h"
#include "ModalTaskHandler.h"

namespace hdps {

ModalTask::ModalTask(QObject* parent, const QString& name, const Status& status /*= Status::Idle*/) :
    Task(parent, name, status, nullptr)
{
    setHandler(new ModalTaskHandler(this));
}

}
