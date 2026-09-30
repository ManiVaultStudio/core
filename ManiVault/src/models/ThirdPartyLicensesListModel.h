// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2025 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "AbstractThirdPartyLicensesModel.h"

namespace mv {

/**
 * List model containing aggregated Core and plugin third-party licenses
 *
 * The model combines the Core dependency list with the metadata declared by all loaded
 * plugin factories. It also tracks which declared plugin dependencies are currently in use
 * by instantiated plugin instances.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ThirdPartyLicensesListModel : public AbstractThirdPartyLicensesModel
{
public:

    /**
     * Construct the model with optional p parent
     * @param parent Parent object
     */
    explicit ThirdPartyLicensesListModel(QObject* parent = nullptr);

    /** Repopulate the model from Core and the plugin manager. */
    void populateFromPluginManager();
};

}
