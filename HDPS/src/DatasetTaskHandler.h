// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractTaskHandler.h"

namespace hdps {

class DatasetTask;

/**
 * Dataset task handler class
 *
 * Interact with dataset task.
 *
 * @author Thomas Kroes
 */
class DatasetTaskHandler final : public AbstractTaskHandler
{
public:

    /**
    * Construct task handler with \p parent object
    * @param parent Pointer to parent object
    * @param datasetTask Pointer to dataset task to handle
    */
    DatasetTaskHandler(QObject* parent, DatasetTask* datasetTask);

    /**
     * Get status bar action
     * @return Pointer to status bar widget action
     */
    gui::WidgetAction* getStatusBarAction() override { return nullptr; }

private:

    /** Get pointer to dataset task */
    DatasetTask* getDatasetTask();
};

}
