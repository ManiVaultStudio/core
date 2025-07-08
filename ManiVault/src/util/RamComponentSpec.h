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
     * Get whether the CPU component specification meets the \p required CPU component specification
     * @param required CPU component spec that is required
     * @return Boolean determining whether the CPU component spec meets the required CPU component spec
     */
    bool meets(const HardwareComponentSpec& required) const override;

    /**
     * Get the reason why the CPU component spec does not meet the \p required CPU component spec
     * @param required CPU component spec that is required
     * @return String containing the reason why the CPU component spec does not meet the required CPU component spec
     */
    QString getFailureString(const HardwareComponentSpec& required) const override;

    /**
     * Get the amount of available RAM in bytes
     * @return Number of bytes of available RAM
     */
    std::uint64_t getNumberOfBytes() const {
        return _numberOfBytes;
    }

    /**
     * Get whether the RAM component specification is equal to the \p other RAM component specification
     * @param other RAM component specification to compare with
     * @return Boolean determining whether the RAM component specification is equal to the \p other RAM component specification
     */
    bool operator==(const RamComponentSpec& other) const {
        return _numberOfBytes == other._numberOfBytes;
    }

    /**
     * Get whether the RAM component specification is not equal to the \p other RAM component specification
     * @param other RAM component specification to compare with
     * @return Boolean determining whether the RAM component specification is not equal to the \p other RAM component specification
     */
    bool operator!=(const RamComponentSpec& other) const {
        return !(*this == other);
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