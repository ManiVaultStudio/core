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
     * Get whether the display component specification meets the \p required display component specification
     * @param required Display component spec that is required
     * @return Boolean determining whether the display component spec meets the required display component spec
     */
    bool meets(const HardwareComponentSpec& required) const;

    /**
     * Get the reason why the display component spec does not meet the \p required display component spec
     * @param required Display component spec that is required
     * @return String containing the reason why the display component spec does not meet the required display component spec
     */
    QString getFailureString(const HardwareComponentSpec& required) const;

    /**
     * Get whether the display component specification is equal to the \p other display component specification
     * @param other Display component specification to compare with
     * @return Boolean determining whether the display component specification is equal to the \p other display component specification
     */
    bool operator==(const DisplayComponentSpec& other) const {
        return _resolution == other._resolution;
    }

    /**
     * Get whether the display component specification is not equal to the \p other display component specification
     * @param other Display component specification to compare with
     * @return Boolean determining whether the display component specification is not equal to the \p other display component specification
     */
    bool operator!=(const DisplayComponentSpec& other) const {
        return !(*this == other);
    }

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