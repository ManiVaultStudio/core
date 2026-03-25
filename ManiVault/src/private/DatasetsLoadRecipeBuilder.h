// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

struct ProjectLoadContext;

using ProjectLoadContextStorage = QtTaskTree::Storage<ProjectLoadContext>;

struct DatasetsLoadContext
{
    struct DatasetEntry
    {
        QVariantMap _datasetMap;            /** The variant map containing the dataset data */
        QString     _datasetId;             /** The ID of the dataset */
        QString     _datasetName;           /** The name of the dataset */
        QString     _pluginKind;            /** The plugin kind of the dataset */
        bool        _isDerived = false;     /** Whether the dataset is derived */
    };

    QString                 _jsonFilePath;              /** The file path of the JSON file containing the data hierarchy structure and dataset data */
    QVariantMap             _dataHierarchyVariantMap;   /** The variant map containing the data hierarchy structure and dataset data */
    QString                 _error;                     /** Error message, if any error occurs during the loading process */
    QVector<DatasetEntry>   _datasetEntries;            /** The list of dataset entries extracted from the hierarchy map, to be loaded in the correct order based on their dependencies */
};

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
    QtTaskTree::Group makeRecipe(ProjectLoadContextStorage& projectLoadContextStorage);

private:

    /**
     * Collect the data hierarchy items from the data hierarchy variant map
     * @return Vector of pointers to the collected data hierarchy items
     */
    QVector<mv::DataHierarchyItem*> collectDatasetItems() const;
};