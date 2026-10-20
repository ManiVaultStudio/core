// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractDatasetsModel.h"

namespace mv
{

/**
 * Datasets list model class
 *
 * Standard item model class for datasets as a list
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT DatasetsListModel : public AbstractDatasetsModel
{
public:

    /**
     * Construct with \p populationMode and pointer to \p parent object
     * @param populationMode Population mode
     * @param parent Pointer to parent object
     */
    DatasetsListModel(PopulationMode populationMode = PopulationMode::Automatic, QObject* parent = nullptr);

private:

    /**
     * Add \p dataset to the model (this method is called when a dataset is added to the manager)
     * @param dataset Smart pointer to dataset to add
     */
    void addDataset(Dataset<DatasetImpl> dataset) override;
};

}
