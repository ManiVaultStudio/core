// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "HardwareComponentSpec.h"

namespace mv::util
{

/**
 * RAM component spec class
 *
 * For encapsulating a RAM hardware specification
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT RamComponentSpec : public HardwareComponentSpec
{
public:

    /** Default constructor */
    RamComponentSpec();

    /**
     * Get the amount of available RAM in bytes
     * @return Number of bytes of available RAM
     */
    std::uint64_t getNumberOfBytes() const {
        return _numberOfBytes;
    }

    /**
     * Get whether the RAM component specification is smaller than the \p other RAM component specification
     * @return Boolean determining whether the RAM component specification is smaller than the \p other RAM component specification
     */
    bool operator<(const RamComponentSpec& other) const {
        return getNumberOfBytes() > other.getNumberOfBytes();
    }

    /**
     * Get whether the RAM component specification is larger than the \p other RAM component specification
     * @return Boolean determining whether the RAM component specification is larger than the \p other RAM component specification
     */
    bool operator>(const RamComponentSpec& other) const {
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

    /**
     * Get the total amount of system RAM in bytes  
     * @return The total number of system RAM in bytes
     */
    static std::uint64_t getTotalSystemRAMBytes();

private:
    std::uint64_t   _numberOfBytes;     /** Number of bytes of available RAM */
};

}