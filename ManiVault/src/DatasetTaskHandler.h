// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractTaskHandler.h"

namespace mv {

class DatasetTask;

/**
 * Dataset task handler class
 *
 * Interact with dataset task.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT DatasetTaskHandler final : public AbstractTaskHandler
{
public:

    /**
    * Construct task handler with \p parent object
    * @param parent Pointer to parent object
    * @param datasetTask Pointer to dataset task to handle
    */
    DatasetTaskHandler(QObject* parent, DatasetTask* datasetTask);

private:

    /** Get pointer to dataset task */
    DatasetTask* getDatasetTask();
};

}
