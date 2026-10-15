// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "HardwareComponentSpec.h"

namespace mv::util
{

/**
 * Storage component spec class
 *
 * For encapsulating a storage hardware specification.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT StorageComponentSpec : public HardwareComponentSpec
{
public:

    /** Default constructor */
    StorageComponentSpec();

    /**
     * Get the reason why the storage component spec does not meet the \p required storage component spec
     * @param required Storage component spec that is required
     * @return String containing the reason why the storage component spec does not meet the required storage component spec
     */
    QString getFailureString(const HardwareComponentSpec& required) const override;

    /**
     * Get the number of available bytes in app data folder
     * @return Number of bytes of available disk space in app data folder
     */
    std::uint64_t getNumberOfBytesAvailableInAppData() const {
        return _numberOfBytesAvailableInAppData;
    }

    /**
     * Get whether the storage component specification is equal to the \p other storage component specification
     * @param other Storage component specification to compare with
     * @return Boolean determining whether the storage component specification is equal to the \p other storage component specification
     */
    bool operator==(const StorageComponentSpec& other) const {
        return getNumberOfBytesAvailableInAppData() == other.getNumberOfBytesAvailableInAppData();
    }

    /**
     * Get whether the storage component specification is not equal to the \p other storage component specification
     * @param other Storage component specification to compare with
     * @return Boolean determining whether the storage component specification is not equal to the \p other storage component specification
     */
    bool operator!=(const StorageComponentSpec& other) const {
        return !(*this == other);
    }

    /**
     * Get whether the storage component specification is smaller than the \p other storage component specification
     * @return Boolean determining whether the storage component specification is smaller than the \p other storage component specification
     */
    bool operator<(const StorageComponentSpec& other) const {
        return getNumberOfBytesAvailableInAppData() < other.getNumberOfBytesAvailableInAppData();
    }

    /**
     * Get whether the storage component specification is larger than the \p other storage component specification
     * @return Boolean determining whether the storage component specification is larger than the \p other storage component specification
     */
    bool operator>(const StorageComponentSpec& other) const {
        return other < *this;
    }

    /**
     * Get whether the storage component specification is smaller than the \p other storage component specification
     * @param other Hardware component specification to compare with
     * @return Boolean determining whether the hardware component specification is smaller than the \p other hardware component specification
     */
    bool lessThan(const HardwareComponentSpec& other) const override {
        if (auto otherStorageComponentSpec = dynamic_cast<const StorageComponentSpec*>(&other))
            return *this < *otherStorageComponentSpec;

        return false; // Cannot compare with non-storage component spec
    }

    /**
     * Get whether the storage component specification is equal to the \p other storage component specification
     * @param other Storage component specification to compare with
     * @return Boolean determining whether the storage component specification is equal to the \p other storage component specification
     */
    bool equals(const HardwareComponentSpec& other) const override {
        if (auto otherStorageComponentSpec = dynamic_cast<const StorageComponentSpec*>(&other))
            return *this == *otherStorageComponentSpec;

        return false; // Cannot compare with non-storage component spec
    }

    /**
     * Get whether the storage component specification meets the \p required storage component specification
     * @param required storage component spec that is required
     * @return Boolean determining whether the storage component spec meets the required storage component spec
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

private:
    std::uint64_t   _numberOfBytesAvailableInAppData;     /** Number of available bytes in app data folder */
};

}