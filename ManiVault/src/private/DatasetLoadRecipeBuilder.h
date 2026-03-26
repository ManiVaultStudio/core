// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QtTaskTree>

#include "DatasetLoadContext.h"

namespace mv
{
    class DataHierarchyItem;
}

class DatasetLoadRecipeBuilder
{
public:

    QtTaskTree::Group makeRecipe(const DatasetLoadContext& datasetLoadContext);

private:

    /**
     * Load the dataset based on the provided data hierarchy item
     * @param datasetLoadContext The context representing the dataset to be loaded
     */
    void loadDataset(const DatasetLoadContext& datasetLoadContext);
};