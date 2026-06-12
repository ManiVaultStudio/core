// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PropertiesSerializer.h"

#include "util/Serialization.h"

#include "workflow/WorkflowExecutionContext.h"

#ifdef _DEBUG
	#define PROPERTIES_SERIALIZER_VERBOSE
#endif

using namespace mv::util;
using namespace mv::workflow;

namespace mv
{

UniqueWorkflowPlan PropertiesSerializer::toVariantMapWorkflow(const QVariantMap& propertiesMap)
{
#ifdef PROPERTIES_SERIALIZER_VERBOSE
    qDebug() << "Serializing properties: " << map.keys();
#endif

    auto plan = std::make_unique<WorkflowPlan>(__FUNCTION__);

    const auto serializeStage = plan->addSequentialStage("Serialize", [propertiesMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) {
        auto optimizedPropertiesMap = saveOptimizedVariant(propertiesMap).toMap();

        optimizedPropertiesMap["PropertiesFormatVersion"] = FormatVersion;

        executionContext->setOutput(optimizedPropertiesMap);
    });

    return plan;
}

UniqueWorkflowPlan PropertiesSerializer::fromVariantMapWorkflow(const QVariantMap& propertiesMap, QVariantMap& destinationPropertiesMap, SharedWorkflowExecutionContext parentContext)
{
#ifdef PROPERTIES_SERIALIZER_VERBOSE
    qDebug() << "Deserializing properties: " << propertiesMap.keys();
#endif

    auto plan = std::make_unique<WorkflowPlan>(__FUNCTION__);

    const auto version = propertiesMap.value("PropertiesFormatVersion").toUInt();

    if (version != FormatVersion)
        throw std::runtime_error("Unsupported properties serialization format version");

    WorkflowPlan::Jobs jobs;

    jobs.emplace_back("Process headers", [propertiesMap, &destinationPropertiesMap](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& jobExecutionContext) {
        destinationPropertiesMap = loadOptimizedVariant(propertiesMap, jobExecutionContext).toMap();
    }, WorkflowPlan::JobThreadAffinity::CurrentWorkerThread, WorkflowPlan::JobProgressMode::Atomic);

    plan->addParallelStage("Preprocessing", jobs);

    return plan;
}

}