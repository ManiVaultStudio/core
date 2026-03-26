// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "DatasetLoadContext.h"

#include <QString>

struct DatasetsLoadContext
{
    QString                 _jsonFilePath;              /** The file path of the JSON file containing the data hierarchy structure and dataset data */
    QVariantMap             _dataHierarchyVariantMap;   /** The variant map containing the data hierarchy structure and dataset data */
    QString                 _error;                     /** Error message, if any error occurs during the loading process */
    DatasetLoadContexts     _datasetContexts;           /** The list of dataset entries extracted from the hierarchy map, to be loaded in the correct order based on their dependencies */
};
