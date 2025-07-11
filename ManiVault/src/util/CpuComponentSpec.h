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
class CORE_EXPORT CpuComponentSpec : public HardwareComponentSpec
{
public:

    /** Default constructor */
    CpuComponentSpec();

    /**
     * Get the reason why the CPU component spec does not meet the \p required CPU component spec
     * @param required CPU component spec that is required
     * @return String containing the reason why the CPU component spec does not meet the required CPU component spec
     */
    QString getFailureString(const HardwareComponentSpec& required) const override;

    /**
     * Get whether the CPU component specification is equal to the \p other CPU component specification
     * @param other CPU component specification to compare with
     * @return Boolean determining whether the CPU component specification is equal to the \p other CPU component specification
     */
    bool operator==(const CpuComponentSpec& other) const {

        // FIXME: Implement actual comparison logic for CpuComponentSpec
        return false;
    }

    /**
     * Get whether the CPU component specification is not equal to the \p other CPU component specification
     * @param other CPU component specification to compare with
     * @return Boolean determining whether the CPU component specification is not equal to the \p other CPU component specification
     */
    bool operator!=(const CpuComponentSpec& other) const {

        // FIXME: Implement actual comparison logic for CpuComponentSpec
        return !(*this == other);
    }

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

    /**
     * Get whether the CPU component specification is smaller than the \p other CPU component specification
     * @param other CPU component specification to compare with
     * @return Boolean determining whether the CPU component specification is smaller than the \p other CPU component specification
     */
    bool lessThan(const HardwareComponentSpec& other) const override {
        if (auto otherCpuComponentSpec = dynamic_cast<const CpuComponentSpec*>(&other))
            return *this < *otherCpuComponentSpec;

        return false;
    }

    /**
     * Get whether the CPU component specification is equal to the \p other CPU component specification
     * @param other CPU component specification to compare with
     * @return Boolean determining whether the CPU component specification is equal to the \p other CPU component specification
     */
    bool equals(const HardwareComponentSpec& other) const override {
        if (auto otherCpuComponentSpec = dynamic_cast<const CpuComponentSpec*>(&other))
            return *this == *otherCpuComponentSpec;

        return false;
    }

    /**
     * Get whether the CPU component specification meets the \p required CPU component specification
     * @param required CPU component spec that is required
     * @return Boolean determining whether the CPU component spec meets the required CPU component spec
     */
    bool meets(const HardwareComponentSpec& required) const override;

    /**
     * Get standard item
     * @return Pointer to standard item representing the hardware component spec
     */
    QStandardItem* getStandardItem() const override;

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