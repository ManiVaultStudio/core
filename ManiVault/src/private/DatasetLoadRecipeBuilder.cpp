// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DatasetLoadRecipeBuilder.h"

#include <DataHierarchyItem.h>

#include <QDebug>

using namespace QtTaskTree;

Group DatasetLoadRecipeBuilder::makeRecipe(mv::DataHierarchyItem& datasetItem)
{
    return Group{
        QSyncTask([this, &datasetItem] {
            loadDataset(datasetItem);
        })
    };
}

void DatasetLoadRecipeBuilder::loadDataset(mv::DataHierarchyItem& datasetItem)
{
    qDebug() << "Loading dataset:" << datasetItem.text();

    // Put your actual dataset loading logic here.
    // Example:
    //
    // auto& datasetAction = datasetItem.getDatasetAction();
    // datasetAction.load();

    qDebug() << "Finished loading dataset:" << datasetItem.text();
}