// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QtTaskTree>

namespace mv
{
    class DataHierarchyItem;
}

/**
 * @brief Builder for creating a recipe to load a dataset based on a data hierarchy item
 */
class DatasetLoadRecipeBuilder
{
public:

    /** Construct builder */
    QtTaskTree::Group makeRecipe(mv::DataHierarchyItem& datasetItem);

private:

    /**
     * Load the dataset based on the provided data hierarchy item
     * @param datasetItem The data hierarchy item representing the dataset to be loaded
     */
    void loadDataset(mv::DataHierarchyItem& datasetItem);
};