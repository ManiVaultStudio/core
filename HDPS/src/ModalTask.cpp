// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ModalTask.h"
#include "ModalTaskHandler.h"

namespace hdps {

using namespace gui;

ModalTaskHandler* ModalTask::modalTaskHandler = nullptr;

ModalTask::ModalTask(QObject* parent, const QString& name, const Status& status /*= Status::Undefined*/, bool mayKill /*= true*/) :
    Task(parent, name, status, mayKill, nullptr)
{
    setScope(Scope::Modal);
}

void ModalTask::createHandler(QObject* parent)
{
    if (ModalTask::modalTaskHandler != nullptr)
        return;

    ModalTask::modalTaskHandler = new ModalTaskHandler(parent);
}

}
