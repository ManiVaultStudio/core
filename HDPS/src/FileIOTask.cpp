// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "FileIOTask.h"
#include "FileIOTaskHandler.h"

namespace hdps {

FileIOTask::FileIOTask(QObject* parent, const QString& name, const Status& status /*= Status::Idle*/) :
    Task(parent, name, status, nullptr)
{
    setHandler(new FileIOTaskHandler(this));
}

}
