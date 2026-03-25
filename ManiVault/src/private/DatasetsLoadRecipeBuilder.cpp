// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DatasetsLoadRecipeBuilder.h"

using namespace QtTaskTree;

QVector<mv::DataHierarchyItem*> DatasetsLoadRecipeBuilder::collectDatasetItems() const
{
    return {};
}

QtTaskTree::Group DatasetsLoadRecipeBuilder::makeRecipe(ProjectLoadContextStorage& projectLoadContextStorage)
{
    return {};
    //const QVector<mv::DataHierarchyItem*> datasetItems = collectDatasetItems();

    //if (datasetItems.isEmpty())
    //    return Group{};

    //QList<GroupItem> parallelItems;
    //parallelItems.reserve(datasetItems.size());

    //for (mv::DataHierarchyItem* datasetItem : datasetItems) {
    //    if (!datasetItem)
    //        continue;

    //    parallelItems.append(_datasetLoadRecipeBuilder.makeRecipe(*datasetItem));
    //}

    //return Group{
    //    Parallel{parallelItems}
    //};
}
