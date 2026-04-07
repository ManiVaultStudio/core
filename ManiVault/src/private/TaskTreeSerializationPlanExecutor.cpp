// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "TaskTreeSerializationPlanExecutor.h"

void TaskTreeSerializationPlanExecutor::execute(mv::util::SerializationPlan& serializationPlan)
{
    qDebug() << __FUNCTION__;

    _workflow = std::make_unique<SerializePlanWorkflow>(serializationPlan, this);

    connect(_workflow.get(), &SerializePlanWorkflow::finished, this, [this, workflowPtr = _workflow.get()](bool success, const QString& error) {
        qDebug() << "Workflow finished with success:" << success << "error:" << error;
    });

    _workflow->start();
}
