// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DatasetsListModel.h"

#include "Application.h"
#include "CoreInterface.h"

#include <util/Exception.h>

#ifdef _DEBUG
    #define DATASETS_LIST_MODEL_VERBOSE
#endif

namespace mv
{

using namespace util;

DatasetsListModel::DatasetsListModel(PopulationMode populationMode /*= PopulationMode::Automatic*/, QObject* parent /*= nullptr*/) :
    AbstractDatasetsModel(populationMode, parent)
{
    setColumnCount(static_cast<int>(Column::Count));

    for (auto column : columnInfo.keys())
        setHorizontalHeaderItem(static_cast<int>(column), new HeaderItem(columnInfo[column]));

    if (getPopulationMode() == AbstractDatasetsModel::PopulationMode::Automatic)
        for (auto dataset : mv::data().getAllDatasets())
            addDataset(dataset);
}

void DatasetsListModel::addDataset(Dataset<DatasetImpl> dataset)
{
    AbstractDatasetsModel::addDataset(dataset);

    try {
        Q_ASSERT(dataset.isValid());

        if (!dataset.isValid())
            throw std::runtime_error("Dataset is not valid");

        auto datasetRow = Row(*this, dataset);
        
        appendRow(datasetRow);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to add dataset to datasets list model", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to add dataset to datasets list model");
    }
}

}
