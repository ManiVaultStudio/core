// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DatasetLoadRecipeBuilder.h"

#include <DataHierarchyItem.h>

#include <QDebug>

using namespace QtTaskTree;

Group DatasetLoadRecipeBuilder::makeRecipe(const DatasetLoadContext& datasetLoadContext)
{
    return Group{
        QSyncTask([this, &datasetLoadContext] {
            loadDataset(datasetLoadContext);
        })
    };
}

void DatasetLoadRecipeBuilder::loadDataset(const DatasetLoadContext& datasetLoadContext)
{
    qDebug() << "Loading dataset:" << datasetLoadContext._datasetName;

    // Put your actual dataset loading logic here.
    // Example:
    //
    // auto& datasetAction = datasetItem.getDatasetAction();
    // datasetAction.load();

    qDebug() << "Finished loading dataset:" << datasetLoadContext._datasetName;
}