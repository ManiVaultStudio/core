// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DatasetTaskHandler.h"
#include "DatasetTask.h"

#include "CoreInterface.h"
#include "Set.h"

namespace mv {

using namespace gui;

DatasetTaskHandler::DatasetTaskHandler(QObject* parent, DatasetTask* datasetTask) :
    AbstractTaskHandler(parent, datasetTask)
{
    Q_ASSERT(datasetTask != nullptr);

    if (datasetTask == nullptr)
        return;

    connect(datasetTask, &Task::statusChanged, this, [this](const Task::Status& previousStatus, const Task::Status& status) -> void {
        switch (status)
        {
            case Task::Status::Idle:
            {
                if (previousStatus == Task::Status::Running || previousStatus == Task::Status::RunningIndeterminate)
                    getDatasetTask()->getDataset()->unlock();

                break;
            }

            case Task::Status::Running:
            case Task::Status::RunningIndeterminate:
            {
                if (previousStatus == Task::Status::Idle || previousStatus == Task::Status::Finished || previousStatus == Task::Status::Aborted)
                    getDatasetTask()->getDataset()->lock();

                break;
            }

            case Task::Status::Finished:
            case Task::Status::Aborted:
            {
                if (previousStatus == Task::Status::Running || previousStatus == Task::Status::RunningIndeterminate)
                    getDatasetTask()->getDataset()->unlock();

                break;
            }

            case Task::Status::Aborting:
                break;

            default:
                break;
        }
    });
}

DatasetTask* DatasetTaskHandler::getDatasetTask()
{
    return static_cast<DatasetTask*>(getTask());
}

}
