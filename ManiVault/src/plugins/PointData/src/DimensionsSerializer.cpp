// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DimensionsSerializer.h"
#include "PointData.h"

#include <CoreInterface.h>

#include <util/WorkflowPlan.h>
#include <util/Serialization.h>

#ifdef _DEBUG
	#define DIMENSIONS_SERIALIZER_VERBOSE
#endif

using namespace mv::util;

void DimensionsSerializer::fromVariantMap(const QVariantMap& pointsMap, PointData* pointData)
{
#ifdef DIMENSIONS_SERIALIZER_VERBOSE
    qDebug() << "Deserializing dimensions: " << pointsMap.keys();
#endif

    Q_ASSERT(pointData);

    if (!pointData)
        throw std::runtime_error("Invalid point data provided for dimensions deserialization");

    const auto version = pointsMap.value("DimensionNames").toMap().value("FormatVersion").toUInt();

    if (version != FormatVersion)
        throw std::runtime_error("Unsupported dimensions serialization format version");

    auto context    = std::make_shared<DimensionsLoadContext>(pointsMap);
    auto fromPlan   = std::make_shared<WorkflowPlan>(__FUNCTION__, context);

    fromPlan->addSequentialStage("Read dimension names", [fromPlan, pointsMap, pointData](WorkflowPlan::Job& job) -> void {
        if (auto context = fromPlan->getWorkflowContextAs<DimensionsLoadContext>()) {
            context->_dimensionNames.reserve(pointsMap.value("DimensionNames").toMap().value("Size").toUInt());

            auto result = populateDataBufferFromVariantMap(pointsMap["DimensionNames"].toMap(), WorkflowPlan::ConcurrencyMode::Parallel);

            result._future.waitForFinished();

            //qDebug() << "data size:" << result._data->size()
            //    << "first bytes:" << result._data->left(32).toHex();

            QDataStream dimensionsDataStream(result._data.get(), QIODevice::ReadOnly);

            dimensionsDataStream >> context->_dimensionNames;

            //qDebug() << "Read dimension names: " << context->_dimensionNames << result._data->size();
        }
    }, WorkflowPlan::JobThreadAffinity::GuiThread);

	fromPlan->addSequentialStage("Set dimension names", [fromPlan, pointData](WorkflowPlan::Job& job) -> void {
        if (auto context = fromPlan->getWorkflowContextAs<DimensionsLoadContext>()) {
	        std::vector<QString> dimensionNames;

			dimensionNames.reserve(pointData->getNumDimensions());

			for (const auto& dimensionName : context->_dimensionNames)
				dimensionNames.emplace_back(dimensionName);

            qDebug() << "Setting dimension names: " << dimensionNames;
			pointData->setDimensionNames(dimensionNames);
        }
	}, WorkflowPlan::JobThreadAffinity::GuiThread);

    fromPlan->executeAsync(mv::projects().getWorkflowPlanExecutor());
}

QVariantMap DimensionsSerializer::toVariantMap(const std::vector<QString>& dimensionNames)
{
#ifdef DIMENSIONS_SERIALIZER_VERBOSE
    qDebug() << "Serializing dimensions: " << dimensionNames;
#endif

    QByteArray dimensionsByteArray;

    QDataStream dimensionsDataStream(&dimensionsByteArray, QIODevice::WriteOnly);

    QStringList dimensionNameList;

    dimensionNameList.reserve(dimensionNames.size());

    for (const auto& dimensionName : dimensionNames) {
        dimensionNameList << dimensionName;
    }

    dimensionsDataStream << dimensionNameList;

    auto result = rawDataToVariantMap((char*)dimensionsByteArray.data(), dimensionsByteArray.size());

    result["FormatVersion"] = FormatVersion;

    return result;
}