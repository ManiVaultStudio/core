// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "DatasetLoadRecipeBuilder.h"
#include "DatasetsLoadContext.h"
#include "DatasetLoadContext.h"

struct ProjectLoadContext;

using ProjectLoadContextStorage = QtTaskTree::Storage<ProjectLoadContext>;

class DatasetsLoadRecipeBuilder
{
public:

    /** Construct builder */
    DatasetsLoadRecipeBuilder() = default;

    /**
     * Make a recipe for loading the data hierarchy based on the data in \p projectLoadContextStorage
     * @param projectLoadContextStorage The storage containing the project load context, which includes the datasets load context
     * @return A QtTaskTree::Group representing the recipe for loading the data hierarchy
     */
    QtTaskTree::Group makeRecipe(const ProjectLoadContextStorage& projectLoadContextStorage);

private:

    /**
     * Load the datasets based on the dataset entries in the datasets load context in \p projectLoadContextStorage
     * @param projectLoadContextStorage The storage containing the project load context, which includes the datasets load context with the dataset entries to be loaded
     * @param derived Whether to load derived datasets (true) or non-derived datasets (false)
     */
    void loadDatasets(const ProjectLoadContextStorage& projectLoadContextStorage, bool derived);

private:
    DatasetLoadRecipeBuilder    _datasetLoadRecipeBuilder;      /** The builder for creating the recipe for loading individual datasets, used in the data hierarchy loading recipe */
};