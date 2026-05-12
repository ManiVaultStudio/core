// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PropertiesSerializer.h"

#ifdef _DEBUG
	#define PROPERTIES_SERIALIZER_VERBOSE
#endif

using namespace mv::util;

namespace mv
{

QVariantMap PropertiesSerializer::toVariantMap(const QVariantMap& map)
{
#ifdef PROPERTIES_SERIALIZER_VERBOSE
    qDebug() << "Serializing properties: " << map.keys();
#endif

    auto result = saveOptimizedVariant(map).toMap();

    result["PropertiesFormatVersion"] = FormatVersion;

    return result;
}

void PropertiesSerializer::fromVariantMap(const QVariantMap& propertiesMap, QVariantMap& destinationPropertiesMap)
{
#ifdef PROPERTIES_SERIALIZER_VERBOSE
    qDebug() << "Deserializing properties: " << propertiesMap.keys();
#endif

    const auto version = propertiesMap.value("PropertiesFormatVersion").toUInt();

    if (version != FormatVersion)
        throw std::runtime_error("Unsupported properties serialization format version");

    auto fromPlan = std::make_shared<WorkflowPlan>(__FUNCTION__);

    WorkflowPlan::Jobs preprocessingJobs;

    preprocessingJobs.emplace_back("Process headers", [propertiesMap, &destinationPropertiesMap](WorkflowPlan::Job& job) {
        destinationPropertiesMap = loadOptimizedVariant(propertiesMap).toMap();
    }, WorkflowPlan::JobThreadAffinity::CurrentWorkerThread, WorkflowPlan::JobProgressMode::Atomic);

    fromPlan->addParallelStage("Preprocessing", preprocessingJobs);
    fromPlan->executeAsync(mv::projects().getWorkflowPlanExecutor());
}

}