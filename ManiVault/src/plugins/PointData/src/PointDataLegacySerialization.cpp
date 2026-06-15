// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PointDataLegacySerialization.h"
#include "PointData.h"
#include "DimensionsPickerAction.h"

#include <util/Serialization.h>

#include <workflow/WorkflowRuntimeScoped.h>

using namespace mv::util;

namespace mv::legacy
{

void PointDataLegacySerializer::fromVariantMapPre150(PointData& pointData, const QVariantMap& variantMap, const workflow::SharedWorkflowExecutionContext& executionContext)
{
    qDebug() << "Deserializing PointData from legacy format (pre-1.5.0). This may result in loss of information if the format has changed significantly since then.";

    variantMapMustContain(variantMap, "Data");
    variantMapMustContain(variantMap, "NumberOfPoints");
    variantMapMustContain(variantMap, "NumberOfDimensions");

    const auto data                 = variantMap["Data"].toMap();
    const auto numberOfPoints       = static_cast<size_t>(variantMap["NumberOfPoints"].toInt());
    const auto numberOfDimensions   = variantMap["NumberOfDimensions"].toUInt();
    const auto numberOfElements     = numberOfPoints * numberOfDimensions;
    const auto elementTypeIndex     = static_cast<PointData::ElementTypeSpecifier>(data["TypeIndex"].toInt());
    const auto rawData              = data["Raw"].toMap();

    bool isDense = true;

    if (variantMap.contains("Dense"))
        isDense = variantMap["Dense"].toBool();

    pointData._isDense          = isDense;
    pointData._numDimensions    = numberOfDimensions;

    if (pointData._isDense)
    {
        pointData.setElementTypeSpecifier(elementTypeIndex);
        pointData.resizeVector(numberOfElements);

        if (numberOfPoints > 0 && numberOfDimensions > 0)
    		populateBytesFromBlobMap(rawData, (char*)pointData.getDataVoidPtr(), pointData.getRawDataSize(), executionContext);
    }
    else
    {
        variantMapMustContain(variantMap, "NumberOfNonZeroElements");

        const auto numberOfNonZeroElements = variantMap["NumberOfNonZeroElements"].toULongLong();

        std::vector<char> bytes((numberOfPoints + 1) * sizeof(size_t) + numberOfNonZeroElements * (sizeof(size_t) + sizeof(float)));

        populateBytesFromBlobMap(rawData, bytes.data(), bytes.size(), executionContext);

        pointData._numRows = static_cast<std::uint64_t>(numberOfPoints); // FIXME should be redundant

        size_t offset = 0;
        std::vector<size_t> rowPointers(numberOfPoints + 1);
        std::memcpy(rowPointers.data(), bytes.data() + offset, rowPointers.size() * sizeof(size_t));

        offset += rowPointers.size() * sizeof(size_t);
        std::vector<size_t> colIndices(numberOfNonZeroElements);
        std::memcpy(colIndices.data(), bytes.data() + offset, colIndices.size() * sizeof(size_t));

        offset += colIndices.size() * sizeof(float);
        std::vector<float> values(numberOfNonZeroElements);
        std::memcpy(values.data(), bytes.data() + offset, values.size() * sizeof(float));

        pointData._sparseData.setData(numberOfPoints, numberOfDimensions, std::move(rowPointers), std::move(colIndices), std::move(values));

        qDebug() << "Loaded sparse data with" << pointData._numRows << "points and" << pointData._numDimensions << "dimensions.";
    }
}

void PointsLegacySerializer::fromVariantMapPre150(Points& points, const QVariantMap& variantMap, const workflow::SharedWorkflowExecutionContext& executionContext)
{
    qDebug() << "Deserializing Points from legacy format (pre-1.5.0). This may result in loss of information if the format has changed significantly since then.";

    variantMapMustContain(variantMap, "DimensionNames");
    variantMapMustContain(variantMap, "Selection");

    // For backwards compatibility, check PluginVersion
    if (variantMap["PluginVersion"] == "No Version" && !variantMap["Full"].toBool())
    {
        points.makeSubsetOf(points.getParent()->getFullDataset<mv::DatasetImpl>());

        qWarning() << "[ManiVault deprecation warning]: This project was saved with an older ManiVault version (<1.0). "
            "Please save the project again to ensure compatibility with newer ManiVault versions. "
            "Future releases may not be able to load this projects otherwise. ";
    }

    // Load raw point data
    if (points.isFull()) {
        auto plan = points.getRawData<PointData>()->fromVariantMapWorkflow(variantMap);

		workflow::WorkflowRuntimeScoped::executeBlocking(std::move(plan), nullptr);
    }
    else
    {
        variantMapMustContain(variantMap, "Indices");

        const auto& indicesMap = variantMap["Indices"].toMap();

        points.indices.resize(indicesMap["Count"].toUInt());

        populateBytesFromBlobMap(indicesMap["Raw"].toMap(), (char*)points.indices.data(), points.indices.size() * sizeof(decltype(points.indices)::value_type), executionContext);
    }

    // Load dimension names
    QStringList dimensionNameList;
    std::vector<QString> dimensionNames;

    // Fetch dimension names from map
    const auto fetchDimensionNames = [&variantMap]() -> QStringList {
        QStringList dimensionNames;

        // Dimension names in byte array format
        QByteArray dimensionsByteArray = bytesFromBlobVariantMap(variantMap["DimensionNames"].toMap());

        // Open input data stream
        QDataStream dimensionsDataStream(&dimensionsByteArray, QIODevice::ReadOnly);

        // Stream the data to the dimension names
        dimensionsDataStream >> dimensionNames;

        return dimensionNames;
        };

    if (variantMap["NumberOfDimensions"].toUInt() > 1000)
        dimensionNameList = fetchDimensionNames();
    else
        dimensionNameList = variantMap["DimensionNames"].toStringList();

    if (dimensionNameList.size() == points.getNumDimensions())
    {
        for (const auto& dimensionName : dimensionNameList)
            dimensionNames.push_back(dimensionName);
    }
    else
    {
        for (std::uint64_t dimensionIndex = 0; dimensionIndex < points.getNumDimensions(); dimensionIndex++)
            dimensionNames.emplace_back(QString("Dim %1").arg(QString::number(dimensionIndex)));
    }

    points.setDimensionNames(dimensionNames);

    if (variantMap.contains("Dimensions")) {
        points._dimensionsPickerAction->fromParentVariantMap(variantMap);
    }

    events().notifyDatasetDataChanged(points);

    // Handle saved selection
    if (points.isFull()) {
        const auto& selectionMap = variantMap["Selection"].toMap();

        const auto count = selectionMap["Count"].toUInt();

        if (count > 0) {
            auto selectionSet = points.getSelection<Points>();

            selectionSet->indices.resize(count);

            populateBytesFromBlobMap(selectionMap["Raw"].toMap(), (char*)selectionSet->indices.data(), selectionSet->indices.size() * sizeof(decltype(selectionSet->indices)::value_type), executionContext);

            events().notifyDatasetDataSelectionChanged(points);
        }
    }
}

}
