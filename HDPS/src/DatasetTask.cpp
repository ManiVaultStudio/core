// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DatasetTask.h"
#include "DatasetTaskHandler.h"

namespace hdps {

DatasetTask::DatasetTask(QObject* parent, const QString& name, const Status& status /*= Status::Undefined*/, bool mayKill /*= false*/) :
    Task(parent, name, status, mayKill, nullptr),
    _dataset()
{
    setHandler(new DatasetTaskHandler(this, this));
}

Dataset<DatasetImpl> DatasetTask::getDataset()
{
    return _dataset;
}

void DatasetTask::setDataset(Dataset<DatasetImpl> dataset)
{
    _dataset = dataset;
}

}
