// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DatasetsLoadRecipeBuilder.h"
#include "ProjectLoadRecipeBuilder.h"

using namespace QtTaskTree;

Group DatasetsLoadRecipeBuilder::makeRecipe(const ProjectLoadContextStorage& projectLoadContextStorage)
{
    return Group{
        QSyncTask([this, projectLoadContextStorage] {
            try {
                loadDatasets(projectLoadContextStorage, false);
            }
            catch (const std::exception& e) {
                projectLoadContextStorage->_error = QString::fromUtf8(e.what());
            }
        }),
        QSyncTask([this, projectLoadContextStorage] {
            try {
                loadDatasets(projectLoadContextStorage, true);
            }
            catch (const std::exception& e) {
                projectLoadContextStorage->_error = QString::fromUtf8(e.what());
            }
        })
    };
}

void DatasetsLoadRecipeBuilder::loadDatasets(const ProjectLoadContextStorage& projectLoadContextStorage, bool derived)
{
    if (derived)
        qDebug() << __FUNCTION__ << "(derived datasets)";
    else
        qDebug() << __FUNCTION__ << "(non-derived datasets)";

    auto& dataHierarchyLoadContext = projectLoadContextStorage->_dataHierarchyLoadContext;

    const auto& contexts = derived ? dataHierarchyLoadContext._derivedDatasetLoadContexts : dataHierarchyLoadContext._nonDerivedDatasetLoadContexts;

    for (const auto& context : contexts) {
        auto nonConstContext = const_cast<DatasetLoadContext*>(context);

        if (!nonConstContext->_dataset.isValid())
            throw std::runtime_error("Dataset is invalid.");

        nonConstContext->_dataset->fromVariantMap(context->_datasetMap);
    }
}
