// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "TaskTreeSerializationPlanExecutor.h"

#include <util/OperationContext.h>
#include <util/OperationContextScope.h>

#ifdef _DEBUG
	#define TASK_TREE_SERIALIZATION_VERBOSE
#endif

void TaskTreeSerializationPlanExecutor::execute(mv::util::SerializationPlan& serializationPlan)
{
#ifdef TASK_TREE_SERIALIZATION_VERBOSE
    printLine("Job started", name, 2);
#endif

    _workflow = std::make_unique<SerializePlanWorkflow>(serializationPlan, this, mv::util::OperationContextScope::getShared());

    QEventLoop loop;

    connect(_workflow.get(), &SerializePlanWorkflow::finished, &loop, &QEventLoop::quit);

    connect(_workflow.get(), &SerializePlanWorkflow::finished, this, [this, workflowPtr = _workflow.get()](bool success, const QString& error) {
#ifdef TASK_TREE_SERIALIZATION_VERBOSE
        qDebug() << "Workflow finished with success:" << success << "error:" << error;
#endif
    });

    _workflow->start();

    loop.exec();
}
