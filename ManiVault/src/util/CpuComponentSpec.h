// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "HardwareComponentSpec.h"

namespace mv::util
{

/**
 * CPU component spec class
 *
 * For encapsulating a CPU hardware specification
 *
 * @author Thomas Kroes
 */
CORE_EXPORT class CpuComponentSpec : public HardwareComponentSpec
{
public:

    /** Default constructor */
    CpuComponentSpec();

    /**
     * Get whether the CPU specification is smaller than the \p other CPU specification
     * @return Boolean determining whether the CPU specification is smaller than the \p other CPU specification
     */
    bool operator<(const CpuComponentSpec& other) const {
        return true;
    }

    /**
     * Get whether the CPU specification is larger than the \p other CPU specification
     * @return Boolean determining whether the CPU specification is larger than the \p other CPU specification
     */
    bool operator>(const CpuComponentSpec& other) const {
        return true;
    }

protected: // Population methods

    /** Load the hardware spec from current system */
    void fromSystem() override;

    /**
     * Load the hardware spec from \p variantMap
     * @param variantMap Variant map containing the hardware spec properties
     */
    void fromVariantMap(const QVariantMap& variantMap) override;
};

}