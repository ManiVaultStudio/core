// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectLoadRecipeBuilder.h"
#include "OpenProjectWorkflow.h"

using namespace mv;

using namespace QtTaskTree;

Group ProjectLoadRecipeBuilder::makeRecipe(const QString& projectJsonPath, ProjectLoadContextStorage& projectLoadContextStorage)
{
    const auto jsonVariantMap           = util::loadJsonToVariantMap(projectJsonPath);
    const auto projectVariantMap        = jsonVariantMap["Project"].toMap();
    const auto dataHierarchyVariantMap  = projectVariantMap["DataHierarchy"].toMap();
    const auto datasetIds               = extractDatasetIds(dataHierarchyVariantMap);

    qDebug() << __FUNCTION__ << "Extracted dataset IDs:" << datasetIds._derivedIds << datasetIds._nonDerivedIds;

    return Group{
        projectLoadContextStorage,

        QSyncTask([projectJsonPath, &projectLoadContextStorage] {
	        auto& context = *projectLoadContextStorage;

        	context = {};

	        //context._dataHierarchyLoadContext._jsonFilePath     = projectJsonPath;
	        //context._workspaceLoadContext._jsonFilePath         = workspaceJsonPath;
	    }),
/*
    	_dataHierarchyLoadRecipeBuilder.makeRecipe(projectLoadContextStorage),
		_workspaceLoadRecipeBuilder.makeRecipe(projectLoadContextStorage),
        _datasetsLoadRecipeBuilder.makeRecipe(projectLoadContextStorage)*/
    };
}

ProjectLoadRecipeBuilder::DatasetIdLists ProjectLoadRecipeBuilder::extractDatasetIds(const QVariantMap& dataHierarchyMap)
{
    DatasetIdLists result;

    result._derivedIds.reserve(dataHierarchyMap.size());
    result._nonDerivedIds.reserve(dataHierarchyMap.size());

    for (const auto& datasetId : dataHierarchyMap.keys()) {
        const auto datasetMap = dataHierarchyMap[datasetId].toMap();
        qDebug() << __FUNCTION__ << "Processing dataset ID:" << datasetId << "with data:" << datasetMap;
        if (!datasetMap.contains("Id"))
            throw std::runtime_error("Dataset missing 'Id'");

        const bool isDerived = datasetMap.value("IsDerived", datasetMap.value("Derived", false)).toBool();

        if (isDerived)
            result._derivedIds.append(datasetId);
        else
            result._nonDerivedIds.append(datasetId);
    }

    return result;
}