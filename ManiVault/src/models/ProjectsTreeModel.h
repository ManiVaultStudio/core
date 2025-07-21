// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "models/AbstractProjectsModel.h"
#include "models/ProjectsModelProject.h"


namespace mv {

/**
 * Projects tree model class
 *
 * Contains grouped project content.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ProjectsTreeModel final : public AbstractProjectsModel
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent object
     * @param mode Mode of the model (automatic/manual)
     * @param parent Pointer to parent object
     */
    ProjectsTreeModel(const PopulationMode& mode = PopulationMode::Automatic, QObject* parent = nullptr);

    /** Synchronize the model with the content of all Data Source Names (DSN) */
    void populateFromDsns() override;
};

}
