// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "DatasetLoadContext.h"

struct DataHierarchyLoadProgress
{
    /** The context for loading the data hierarchy, containing the hierarchy map and dataset entries to be loaded, as well as any error message if an error occurs during the loading process */
    using DatasetLoadedCallbackFn = std::function<void(std::int32_t, std::int32_t, const QString&)>;

    std::atomic<std::int32_t>   _datasetsLoaded{ 0 };
    std::int32_t                _totalDatasets = 0;
    DatasetLoadedCallbackFn     _datasetLoadedCallback;
};

using SharedDataHierarchyLoadProgress = std::shared_ptr<DataHierarchyLoadProgress>;

/** Context for loading the data hierarchy, containing the hierarchy map and dataset entries to be loaded, as well as any error message if an error occurs during the loading process */
struct DataHierarchyLoadContext
{
    

    QString                             _jsonFilePath;                      /** The file path of the JSON file containing the data hierarchy structure and dataset data */
    QVariantMap                         _dataHierarchyVariantMap;           /** The variant map containing the data hierarchy structure and dataset data */
    QString                             _error;                             /** Error message, if any error occurs during the loading process */
    DatasetLoadContexts                 _datasetLoadContexts;               /** The list of dataset entries extracted from the hierarchy map, to be loaded in the correct order based on their dependencies */
    DatasetLoadContextsPtrs             _nonDerivedDatasetLoadContexts;     /** The list of non-derived dataset entries extracted from the hierarchy map, to be loaded in the correct order based on their dependencies */
    DatasetLoadContextsPtrs             _derivedDatasetLoadContexts;        /** The list of derived dataset entries extracted from the hierarchy map, to be loaded in the correct order based on their dependencies */
    SharedDataHierarchyLoadProgress     _progress;                          /** Shared pointer to the progress of loading the data hierarchy, allowing for tracking the number of datasets loaded and providing callbacks for when a dataset is loaded */
};

/** Storage for data hierarchy load context */
using DataHierarchyLoadContextStorage = QtTaskTree::Storage<DataHierarchyLoadContext>;
