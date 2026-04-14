// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "TaskTreeSerializationPlanExecutor.h"

#include <util/OperationContext.h>
#include <util/OperationContextScope.h>

#ifdef _DEBUG
	#define TASK_TREE_SERIALIZATION_VERBOSE
#endif

void TaskTreeSerializationPlanExecutor::runWorkflowOnOwnerThread(const mv::util::SerializationPlan& serializationPlan)
{
}

void TaskTreeSerializationPlanExecutor::execute(mv::util::SerializationPlan& serializationPlan)
{
    std::promise<mv::util::AbstractWorkflow::WorkflowOutcome> donePromise;
    auto doneFuture = donePromise.get_future();

    std::exception_ptr errorPtr;
    QString errorMessage;

    _workflow = std::make_unique<SerializePlanWorkflow>(
        serializationPlan,
        mv::util::OperationContextScope::getShared()
    );

    _workflow->setDoneCallback(
        [&](bool success, const QString& error) {
            if (!success) {
                errorMessage = error;
            }

            donePromise.set_value({ success, error });
        }
    );

    _workflow->start();

    doneFuture.wait();

    if (!errorMessage.isEmpty()) {
        throw std::runtime_error(errorMessage.toStdString());
    }
}
