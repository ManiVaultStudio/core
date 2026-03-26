// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DatasetsLoadRecipeBuilder.h"
#include "ProjectLoadRecipeBuilder.h"

using namespace QtTaskTree;

Group DatasetsLoadRecipeBuilder::makeRecipe(const ProjectLoadContextStorage& projectLoadContextStorage)
{
    return Group{
        //QSyncTask([this, projectLoadContextStorage] {
        //    qDebug() << __FUNCTION__;
        //    auto& datasetLoadContexts = projectLoadContextStorage->_dataHierarchyLoadContext._datasetLoadContexts;

        //    //loadNonDerivedDatasets(datasetLoadContexts);
        //    //loadDerivedDatasets(datasetLoadContexts);
        //}),
        //QSyncTask([this, projectLoadContextStorage] {
        //    qDebug() << __FUNCTION__;
        //    auto& datasetLoadContexts = projectLoadContextStorage->_dataHierarchyLoadContext._datasetLoadContexts;
        //    qDebug() << datasetLoadContexts.size() << "datasets to load";

        //    //loadNonDerivedDatasets(datasetLoadContexts);
        //    //loadDerivedDatasets(datasetLoadContexts);
        //    makeDatasetJobsRecipe(projectLoadContextStorage);
        //})//,
        //makeDatasetJobsRecipe(projectLoadContextStorage)
    };



    //auto& datasetEntries = projectLoadContextStorage->_datasetsLoadContext._datasetContexts;

    //const auto nonDerivedDatasetEntries = collectDatasetItems(datasetEntries);
    //const auto derivedDatasetEntries    = collectDatasetItems(datasetEntries, true);

    ////if (nonDerivedDatasetEntries.isEmpty() && derivedDatasetEntries.isEmpty())
    ////    return Group{};

    //QList<GroupItem> nonDerivedParallelItems;
    //nonDerivedParallelItems.reserve(nonDerivedDatasetEntries.size());

    //for (auto nonDerivedDatasetEntry : nonDerivedDatasetEntries) {
    //    if (!nonDerivedDatasetEntry)
    //        continue;

    //    nonDerivedParallelItems.append(_datasetLoadRecipeBuilder.makeRecipe(*nonDerivedDatasetEntry));
    //}

    //QList<GroupItem> derivedParallelItems;
    //derivedParallelItems.reserve(derivedDatasetEntries.size());

    //for (auto derivedDatasetEntry : derivedDatasetEntries) {
    //    if (!derivedDatasetEntry)
    //        continue;

    //    derivedParallelItems.append(_datasetLoadRecipeBuilder.makeRecipe(*derivedDatasetEntry));
    //}

    //return Group{
    //    parallel,
    //	nonDerivedParallelItems,
    //	derivedParallelItems
    //};
}

void DatasetsLoadRecipeBuilder::loadNonDerivedDatasets(DatasetLoadContexts& datasetLoadContexts)
{
    qDebug() << __FUNCTION__;

    for (auto& datasetLoadContext : datasetLoadContexts) {
        if (!datasetLoadContext._isDerived)
            datasetLoadContext._dataset->fromVariantMap(datasetLoadContext._datasetMap);
    }
}

void DatasetsLoadRecipeBuilder::loadDerivedDatasets(DatasetLoadContexts& datasetLoadContexts)
{
    qDebug() << __FUNCTION__;

    for (auto& datasetLoadContext : datasetLoadContexts) {
        if (datasetLoadContext._isDerived)
            datasetLoadContext._dataset->fromVariantMap(datasetLoadContext._datasetMap);
    }
}

Group DatasetsLoadRecipeBuilder::makeDatasetJobsRecipe(const ProjectLoadContextStorage& projectLoadContextStorage)
{
    qDebug() << __FUNCTION__;

    auto& datasetLoadContexts = projectLoadContextStorage->_dataHierarchyLoadContext._datasetLoadContexts;

    qDebug() << datasetLoadContexts.size() << "datasets to load";
    GroupItems items{ parallel };

    for (auto& datasetLoadContext : datasetLoadContexts) {
        if (datasetLoadContext._isDerived)
            items << _datasetLoadRecipeBuilder.makeRecipe(datasetLoadContext);
    }

    return Group(items);
}