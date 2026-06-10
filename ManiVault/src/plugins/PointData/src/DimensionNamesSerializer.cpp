// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DimensionNamesSerializer.h"
#include "PointData.h"

#include <CoreInterface.h>

#include <util/WorkflowPlan.h>
#include <util/Serialization.h>

#ifdef _DEBUG
	#define DIMENSION_NAMES_SERIALIZER_VERBOSE
#endif

using namespace mv;
using namespace mv::util;

UniqueWorkflowPlan DimensionNamesSerializer::fromVariantMapWorkflow(Points* points, const QVariantMap& variantMap, mv::util::SharedWorkflowExecutionContext parentContext /*= nullptr*/)
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

    auto context    = std::make_shared<DimensionNamesLoadContext>();
    auto fromPlan   = std::make_unique<WorkflowPlan>(__FUNCTION__, context);

    fromPlan->addSequentialStage("Read dimension names", [context, variantMap, points](const WorkflowPlan::Job& job) -> void {
        context->_dimensionNames.reserve(variantMap.value("DimensionNames").toMap().value("Size").toUInt());

        auto bytes = bytesFromBlobVariantMap(variantMap["DimensionNames"].toMap());

        QDataStream dimensionsDataStream(&bytes, QIODevice::ReadOnly);

        dimensionsDataStream >> context->_dimensionNames;
    }, WorkflowPlan::JobThreadAffinity::GuiThread);

	fromPlan->addSequentialStage("Set dimension names", [context, points](const WorkflowPlan::Job& job) -> void {
        std::vector<QString> dimensionNames;

		dimensionNames.reserve(points->getNumDimensions());

        for (const auto& dimensionName : context->_dimensionNames)
			dimensionNames.emplace_back(dimensionName);

		points->setDimensionNames(dimensionNames);
	}, WorkflowPlan::JobThreadAffinity::GuiThread);

    return fromPlan;
}

mv::util::UniqueWorkflowPlan DimensionNamesSerializer::toVariantMapWorkflow(const std::vector<QString>& dimensionNames)
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