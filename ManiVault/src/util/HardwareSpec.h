// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "Serializable.h"
#include "HardwareSpec.h"
#include "HardwareComponentSpec.h"

#include <QString>

namespace mv::util
{

class CORE_EXPORT HardwareSpec : public Serializable
{
public:

    /**
     * Load the hardware spec from \p variantMap
     * @param variantMap Variant map containing the hardware spec properties
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /** Initialize from system hardware specifications */
    void fromSystem();

    /**
     * Get whether the hardware specification is smaller than the \p other hardware specification
     * @return Boolean determining whether the hardware specification is smaller than the \p other hardware specification
     */
    bool operator<(const HardwareSpec& other) const {
        std::vector<bool> conditions;

        conditions.reserve(_componentSpecs.size());

        std::transform(_componentSpecs.begin(), _componentSpecs.end(), other._componentSpecs.begin(), std::back_inserter(conditions),
            [](const auto& lhs, const auto& rhs) {
                return lhs < rhs;
		});

        return std::ranges::all_of(conditions.begin(), conditions.end(), [](bool condition) { return condition; });
    }

    /**
     * Get whether the hardware specification is larger than the \p other hardware specification
     * @return Boolean determining whether the hardware specification is larger than the \p other hardware specification
     */
    bool operator>(const HardwareSpec& other) const {
        return other < *this;
    }

public: // System hardware specification

	/**
     * Get system hardware specification
     * @return System hardware specification
     */
    static const HardwareSpec& getSystemHardwareSpec();

    /** Updates the system hardware specifications */
    static void updateSystemHardwareSpecs();

private:
    std::vector<std::shared_ptr<HardwareComponentSpec>>   _componentSpecs;      /** Hardware component specifications */

    static HardwareSpec systemHardwareSpec; /** System hardware specification */
};

}
