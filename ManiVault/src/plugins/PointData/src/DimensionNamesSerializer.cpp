// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DimensionNamesSerializer.h"
#include "PointData.h"

#include <CoreInterface.h>

#include <workflow/WorkflowPlan.h>

#include <util/Serialization.h>

#ifdef _DEBUG
	#define DIMENSION_NAMES_SERIALIZER_VERBOSE
#endif

using namespace mv;
using namespace mv::util;
using namespace mv::workflow;

UniqueWorkflowPlan DimensionNamesSerializer::fromVariantMapWorkflow(Points* points, const QVariantMap& variantMap, SharedWorkflowExecutionContext parentContext /*= nullptr*/)
{
#ifdef DIMENSIONS_SERIALIZER_VERBOSE
    qDebug() << "Deserializing dimensions: " << pointsMap.keys();
#endif

    Q_ASSERT(points);

    if (!points)
        throw std::runtime_error("Invalid points provided for dimension names deserialization");

    const auto version = variantMap.value("DimensionNames").toMap().value("FormatVersion").toUInt();

    if (version != FormatVersion)
        throw std::runtime_error("Unsupported dimensions serialization format version");

    // Context struct to hold intermediate data during workflow execution
    struct Context : WorkflowContextBase {
        QStringList     dimensionNames;
        QByteArray      bytes;
    };

    auto context    = std::make_shared<Context>();
    auto plan       = std::make_unique<WorkflowPlan>(__FUNCTION__, context);

    const auto dimensionNamesMap = variantMap["DimensionNames"].toMap();

    plan->addNestedWorkflowStage("Load dimension data", [dimensionNamesMap, context](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) -> UniqueWorkflowPlan {
        context->bytes.resize(dimensionNamesMap["Size"].toUInt());
        return populateBytesFromBlobMapWorkflow(dimensionNamesMap, context->bytes.data(), context->bytes.size(), executionContext);
    });

    plan->addSequentialStage("Read dimension names", [context, dimensionNamesMap, points](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) -> void {
        context->dimensionNames.reserve(dimensionNamesMap["Size"].toUInt());

        QDataStream dimensionsDataStream(&context->bytes, QIODevice::ReadOnly);

        dimensionsDataStream >> context->dimensionNames;
    });

	plan->addSequentialStage("Set dimension names", [context, points](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) -> void {
        std::vector<QString> dimensionNames;

		dimensionNames.reserve(points->getNumDimensions());

        for (const auto& dimensionName : context->dimensionNames)
			dimensionNames.emplace_back(dimensionName);

		points->setDimensionNames(dimensionNames);
	}, WorkflowPlan::JobThreadAffinity::GuiThread);

    return plan;
}

UniqueWorkflowPlan DimensionNamesSerializer::toVariantMapWorkflow(const std::vector<QString>& dimensionNames)
{
#ifdef DIMENSIONS_SERIALIZER_VERBOSE
    qDebug() << "Serializing dimensions: " << dimensionNames;
#endif

    auto plan = std::make_unique<WorkflowPlan>(__FUNCTION__);

    plan->addSequentialStage("Read cluster data", [dimensionNames](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) {
        QByteArray dimensionsByteArray;

        QDataStream dimensionsDataStream(&dimensionsByteArray, QIODevice::WriteOnly);

        QStringList dimensionNameList;

        dimensionNameList.reserve(static_cast<qsizetype>(dimensionNames.size()));

        for (const auto& dimensionName : dimensionNames) {
            dimensionNameList << dimensionName;
        }

        dimensionsDataStream << dimensionNameList;

        auto outputMap = bytesToBlobVariantMap((char*)dimensionsByteArray.data(), dimensionsByteArray.size(), executionContext);

        outputMap["FormatVersion"] = FormatVersion;

        executionContext->setOutput(outputMap);
    });

    return plan;
}