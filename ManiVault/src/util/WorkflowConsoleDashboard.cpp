// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowExecutionNodeType.h"

#include <QHash>

namespace mv::util
{

WorkflowExecutionNodeType getWorkflowExecutionNodeType(const QString& typeName)
{
    static const QHash<QString, WorkflowExecutionNodeType> map = {
        { QStringLiteral("Workflow"), WorkflowExecutionNodeType::Workflow },
        { QStringLiteral("NestedWorkflow"), WorkflowExecutionNodeType::NestedWorkflow },
        { QStringLiteral("SequentialStage"), WorkflowExecutionNodeType::SequentialStage },
        { QStringLiteral("ParallelStage"), WorkflowExecutionNodeType::ParallelStage },
        { QStringLiteral("Job"), WorkflowExecutionNodeType::Job },
        { QStringLiteral("Undefined"), WorkflowExecutionNodeType::Undefined }
    };

    return map.value(typeName, WorkflowExecutionNodeType::Undefined);
}

QString getWorkflowExecutionNodeTypeName(WorkflowExecutionNodeType type)
{
    switch (type) {
	    case WorkflowExecutionNodeType::Workflow:
	        return QStringLiteral("Workflow");

	    case WorkflowExecutionNodeType::NestedWorkflow:
	        return QStringLiteral("NestedWorkflow");

	    case WorkflowExecutionNodeType::SequentialStage:
	        return QStringLiteral("SequentialStage");

	    case WorkflowExecutionNodeType::ParallelStage:
	        return QStringLiteral("ParallelStage");

	    case WorkflowExecutionNodeType::Job:
	        return QStringLiteral("Job");

	    case WorkflowExecutionNodeType::Undefined:

	    default:
	        return QStringLiteral("Undefined");
    }
}

}
