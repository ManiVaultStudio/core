// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "DataHierarchyLoadContext.h"
#include "DatasetLoadContext.h"

namespace mv
{
    /* Forward declaration of DataHierarchyManager to avoid circular dependency with DataHierarchyLoadRecipeBuilder */
    class DataHierarchyManager;
}

struct ProjectLoadContext;

using ProjectLoadContextStorage = QtTaskTree::Storage<ProjectLoadContext>;

/**
 * Builder for creating a recipe to load the data hierarchy based on the data in a DataHierarchyLoadContextStorage
 */
class DataHierarchyLoadRecipeBuilder
{
public:

    /** Construct builder */
    DataHierarchyLoadRecipeBuilder() = default;

    /**
     * Make a recipe for loading the data hierarchy based on the data in \p projectLoadContextStorage
     * @param projectLoadContextStorage The storage containing the project load context, which includes the data hierarchy load context
     * @return A QtTaskTree::Group representing the recipe for loading the data hierarchy
     */
    QtTaskTree::Group makeRecipe(ProjectLoadContextStorage& projectLoadContextStorage);

    /**
     * Load the project JSON file and populate the project load context with the data from the JSON file
     * @param dataHierarchyLoadContext The context to populate with the project load data
     */
    void loadProjectJson(DataHierarchyLoadContext& dataHierarchyLoadContext);

    /**
     * Enumerate the datasets in the hierarchy map and populate the dataset entries in \p dataHierarchyLoadContext
     * @param dataHierarchyLoadContext The context to populate with the dataset entries
     */
    void enumerateDatasets(DataHierarchyLoadContext& dataHierarchyLoadContext);

    /**
     * Compute the load order of the dataset entries in \p dataHierarchyLoadContext based on their dependencies, and populate the dataset load contexts in the context with the dataset entries in the correct order
     * @param dataHierarchyLoadContext The context containing the dataset entries to compute the load order for and to populate with the ordered dataset entries
     */
    void partitionDatasetLoadContexts(DataHierarchyLoadContext& dataHierarchyLoadContext);

    /**
     * Populate the data hierarchy based on the hierarchy map in \p dataHierarchyLoadContext
     * @param dataHierarchyLoadContext The context containing the hierarchy map to populate the data hierarchy from
     */
    void populateHierarchy(DataHierarchyLoadContext& dataHierarchyLoadContext);

    /**
     * Recursively populate the data hierarchy based on the hierarchy map
     * @param dataHierarchyLoadContext The context containing the hierarchy map and dataset entries to populate the data hierarchy from
     * @param map The hierarchy map to populate from
     * @param parent The parent dataset for the current level of the hierarchy
     */
    void populateHierarchy(DataHierarchyLoadContext& dataHierarchyLoadContext, const QVariantMap& map, const mv::Dataset<>& parent);

    /**
     * Load a dataset and its hierarchy item from the given item map
     * @param dataHierarchyLoadContext The context containing the dataset entries to load and any error message if an error occurs during the loading process
     * @param itemMap The variant map containing the dataset and hierarchy item data
     * @param guiName The GUI name to assign to the loaded dataset
     * @param parent The parent dataset for the dataset being loaded
     * @return The loaded dataset
     */
    mv::Dataset<> loadDataHierarchyItem(DataHierarchyLoadContext& dataHierarchyLoadContext, const QVariantMap& itemMap, const QString& guiName, const mv::Dataset<>& parent);

    /**
     * Make a QtTaskTree::Group representing the stage of loading the datasets in the data hierarchy, based on the dataset entries in \p dataHierarchyLoadContextStorage
     * @param dataHierarchyLoadContextStorage The storage containing the data hierarchy load context with the dataset entries to load
     * @return A QtTaskTree::Group representing the dataset loading stage
     */
    QtTaskTree::Group makeDatasetLoadStage(DataHierarchyLoadContextStorage& dataHierarchyLoadContextStorage);

private:
    DataHierarchyLoadContextStorage     _dataHierarchyLoadContextStorage;       /** Storage for the data hierarchy load context, which contains the dataset entries to be loaded in the correct order based on their dependencies */
};