// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "HardwareComponentSpec.h"

namespace mv::util
{

/**
 * Display component spec class
 *
 * For encapsulating a display hardware specification
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT DisplayComponentSpec : public HardwareComponentSpec
{
public:

    /** Default constructor */
    DisplayComponentSpec();

    /**
     * Get whether the display component specification is smaller than the \p other display component spec
     * @return Boolean determining whether the display component specification is smaller than the \p other display component spec
     */
    bool operator<(const DisplayComponentSpec& other) const {
        return _resolution.first > other._resolution.first && _resolution.second > other._resolution.second;
    }

    /**
     * Get whether the display component specification is larger than the \p other display component spec
     * @return Boolean determining whether the display component specification is larger than the \p other display component spec
     */
    bool operator>(const DisplayComponentSpec& other) const {
        return other < *this;
    }

protected: // Population methods

    /** Load the hardware spec from current system */
    void fromSystem() override;

    /**
     * Load the hardware spec from \p variantMap
     * @param variantMap Variant map containing the hardware spec properties
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

private:
    std::pair<std::int32_t, std::int32_t>    _resolution;    /** Display resolution */
};

}