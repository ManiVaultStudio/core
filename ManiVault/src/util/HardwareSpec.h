// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "HardwareSpec.h"
#include "HardwareComponentSpec.h"

#include <QString>

namespace mv::util
{

class CORE_EXPORT HardwareSpec
{
public:

    /** Default constructor */
    HardwareSpec();

    /**
     * Load the hardware spec from \p variantMap
     * @param variantMap Variant map containing the hardware spec properties
     */
    void fromVariantMap(const QVariantMap& variantMap);

    /** Initialize from system hardware specifications */
    void fromSystem() const;

    /**
     * Get whether the hardware spec has been initialized
     * @return Boolean determining whether the hardware spec has been initialized
     */
    bool isInitialized() const;

    /**
     * Get the failing hardware component specifications
     * @param other Other hardware specification to compare with
     * @return Failing hardware component specifications
     */
    HardwareComponentSpecs getFailingHardwareComponentSpecs(const HardwareSpec& other) const;

public: // Conditional

    /**
     * Get whether the hardware specification is equal to the \p other hardware specification
     * @param other Other hardware specification to compare with
     * @return Boolean determining whether the hardware specification is equal to the \p other hardware specification
     */
    bool operator==(const HardwareSpec& other) const {
        if (_componentSpecs.size() != other._componentSpecs.size())
            return false;

        for (std::size_t i = 0; i < _componentSpecs.size(); ++i) {
            const auto& lhs = _componentSpecs[i];
            const auto& rhs = other._componentSpecs[i];

            if (!lhs->isInitialized() || !rhs->isInitialized())
                continue;

            if (!lhs->meets(*rhs))
                return false;
        }

        return true;
    }

    /**
     * Get whether the hardware specification is not equal to the \p other hardware specification
     * @param other Other hardware specification to compare with
     * @return Boolean determining whether the hardware specification is not equal to the \p other hardware specification
     */
    bool operator!=(const HardwareSpec& other) const {
        return !(*this == other);
    }

    /**
     * Get whether the hardware specification is smaller than the \p other hardware specification
     * @return Boolean determining whether the hardware specification is smaller than the \p other hardware specification
     */
    bool operator<(const HardwareSpec& other) const {
        std::vector<bool> conditions;

        conditions.reserve(_componentSpecs.size());

        for (std::size_t i = 0; i < _componentSpecs.size() && i < other._componentSpecs.size(); ++i) {
            const auto& lhs = _componentSpecs[i];
            const auto& rhs = other._componentSpecs[i];

            if (lhs->isInitialized() && rhs->isInitialized())
                conditions.push_back(lhs < rhs);
        }

        return std::ranges::all_of(conditions.begin(), conditions.end(), [](bool condition) { return condition; });
    }

    /**
     * Get whether the hardware specification is larger than the \p other hardware specification
     * @return Boolean determining whether the hardware specification is larger than the \p other hardware specification
     */
    bool operator>(const HardwareSpec& other) const {
        return other < *this;
    }

    /**
     * Get whether the hardware specification is equal to or smaller than the \p other hardware specification
     * @param other Other hardware specification to compare with
     * @return Boolean determining whether the hardware specification is equal to or smaller than the \p other hardware specification
     */
    bool operator<=(const HardwareSpec& other) const {
        return *this < other || *this == other;
    }

    /**
     * Get whether the hardware specification is equal to or larger than the \p other hardware specification
     * @param other Other hardware specification to compare with
     * @return Boolean determining whether the hardware specification is equal to or larger than the \p other hardware specification
     */
    bool operator>=(const HardwareSpec& other) const {
        return *this > other || *this == other;
    }

    /**
     * Get whether the hardware specification meets the \p required hardware specification
     * @param required Hardware spec that is required
     * @return Boolean determining whether the hardware spec meets the required hardware spec
     */
    bool meets(const HardwareSpec& required) const;

    /**
     * Get the reason why the hardware component spec does not meet the \p required hardware component spec
     * @param required Hardware component spec that is required
     * @return String containing the reason why the hardware component spec does not meet the required hardware component spec
     */
    QString getFailureString(const HardwareSpec& required) const;

public: // System hardware specification

	/**
     * Get system hardware specification
     * @return System hardware specification
     */
    static const HardwareSpec& getSystemHardwareSpec();

    /** Updates the system hardware specifications */
    static void updateSystemHardwareSpecs();

private:
    HardwareComponentSpecs  _componentSpecs;    /** Hardware component specifications */

    static HardwareSpec systemHardwareSpec; /** System hardware specification */
};

}
