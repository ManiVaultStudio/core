// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QString>

struct DatasetLoadContext
{
	mv::Dataset<>   _dataset;               /** The dataset to be loaded */
    QVariantMap     _datasetMap;            /** The variant map containing the dataset data */
    QString         _datasetId;             /** The ID of the dataset */
    QString         _datasetName;           /** The name of the dataset */
    QString         _pluginKind;            /** The plugin kind of the dataset */
    bool            _isDerived = false;     /** Whether the dataset is derived */
};

using DatasetLoadContexts = QVector<DatasetLoadContext>;
using DatasetLoadContextsPtrs = QVector<DatasetLoadContext*>;
