// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DatasetsMimeData.h"

namespace mv {

DatasetsMimeData::DatasetsMimeData(Datasets datasets) :
    QMimeData(),
    _datasets(datasets)
{
}

QStringList DatasetsMimeData::formats() const
{
    return { format() };
}

Datasets DatasetsMimeData::getDatasets() const
{
    return _datasets;
}

QString DatasetsMimeData::format()
{
    return "application/datasets";
}

}
