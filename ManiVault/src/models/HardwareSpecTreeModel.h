// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "models/AbstractHardwareSpecModel.h"

#include "util/HardwareSpec.h"

namespace mv {

/**
 * Hardware specs tree model class
 *
 * Contains hardware specifications in a tree model.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT HardwareSpecTreeModel final : public AbstractHardwareSpecModel
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    HardwareSpecTreeModel(QObject* parent = nullptr);

    /**
     * Set the hardware specification to be displayed in the model
     * @param hardwareSpec The hardware specification to set
     */
    void setHardwareSpec(const util::HardwareSpec& hardwareSpec);

private:
};

}
