// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

namespace mv
{
    /* Forward declaration of DataHierarchyManager to avoid circular dependency with DataHierarchyLoadRecipeBuilder */
    class DataHierarchyManager;
}

/** Context for loading the data hierarchy, containing the hierarchy map and dataset entries to be loaded, as well as any error message if an error occurs during the loading process */
struct DataHierarchyLoadContext
{
    /** Represents an entry for a dataset to be loaded, containing the dataset data and metadata extracted from the hierarchy map, as well as whether the dataset is derived or not (to determine load order) */
    struct DatasetEntry
    {
        QVariantMap _datasetMap;            /** The variant map containing the dataset data */
        QString     _datasetId;             /** The ID of the dataset */
        QString     _datasetName;           /** The name of the dataset */
        QString     _pluginKind;            /** The plugin kind of the dataset */
        bool        _isDerived = false;     /** Whether the dataset is derived */
    };

    QVariantMap             _hierarchyMap;      /** The variant map containing the data hierarchy structure and dataset data */
    QString                 _error;             /** Error message, if any error occurs during the loading process */
    QVector<DatasetEntry>   _datasetEntries;    /** The list of dataset entries extracted from the hierarchy map, to be loaded in the correct order based on their dependencies */
};

/** Storage for data hierarchy load context */
using DataHierarchyLoadContextStorage = QtTaskTree::Storage<DataHierarchyLoadContext>;

/**
 * Builder for creating a recipe to load the data hierarchy based on the data in a DataHierarchyLoadContextStorage
 */
class DataHierarchyLoadRecipeBuilder
{
public:

    /** Construct builder */
    explicit DataHierarchyLoadRecipeBuilder();

    /**
     * Make a recipe for loading the data hierarchy based on the data in \p dataHierarchyLoadContextStorage
     * @param dataHierarchyLoadContextStorage The storage containing the data hierarchy load context
     * @return A QtTaskTree::Group representing the recipe for loading the data hierarchy
     */
    QtTaskTree::Group makeRecipe(DataHierarchyLoadContextStorage& dataHierarchyLoadContextStorage);

private:

    /**
     * Enumerate the datasets in the hierarchy map and populate the dataset entries in \p dataHierarchyLoadContext
     * @param dataHierarchyLoadContext The context to populate with the dataset entries
     */
    void enumerateDatasets(DataHierarchyLoadContext& dataHierarchyLoadContext);

    /**
     * Recursively populate the data hierarchy based on the hierarchy map
     * @param map The hierarchy map to populate from
     * @param parent The parent dataset for the current level of the hierarchy
     */
    void populateHierarchy(const QVariantMap& map, const mv::Dataset<>& parent);

    /**
     * Compute the load order of the datasets based on their dependencies, ensuring that parent datasets are loaded before their children
     * @param dataHierarchyLoadContext The context containing the dataset entries to compute the load order for
     * @return A vector of dataset entries in the order they should be loaded
     */
    QVector<DataHierarchyLoadContext::DatasetEntry> computeLoadOrder(const DataHierarchyLoadContext& dataHierarchyLoadContext);

    /**
     * Load a dataset and its hierarchy item from the given item map
     * @param itemMap The variant map containing the dataset and hierarchy item data
     * @param guiName The GUI name to assign to the loaded dataset
     * @param parent The parent dataset for the dataset being loaded
     * @return The loaded dataset
     */
    mv::Dataset<> loadDataHierarchyItem(const QVariantMap& itemMap, const QString& guiName, const mv::Dataset<>& parent);

    /**
     * Make a QtTaskTree::Group representing the stage of loading the datasets in the data hierarchy, based on the dataset entries in \p dataHierarchyLoadContextStorage
     * @param dataHierarchyLoadContextStorage The storage containing the data hierarchy load context with the dataset entries to load
     * @return A QtTaskTree::Group representing the dataset loading stage
     */
    QtTaskTree::Group makeDatasetLoadStage(DataHierarchyLoadContextStorage& dataHierarchyLoadContextStorage);
};