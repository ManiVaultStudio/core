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

    /** Hardware specification type */
    enum class Type {
        System,         /** System hardware specification */
        Minimum,        /** Minimum hardware specification */
        Recommended     /** Recommended hardware specification */
    };

    /** Enum for system hardware spec compatibility with other specifications */
    enum class SystemCompatibility {
        Incompatible,   /** Incompatible with the minimum and recommended hardware specification */
        Minimum,        /** Compatible with the minimum hardware specification only */
        Compatible,     /** Compatible with the minimum and recommended hardware specification */
        Unknown         /** Unknown compatibility */
    };

    /** System compatibility information */
    struct SystemCompatibilityInfo {
        SystemCompatibility     _compatibility;     /** System compatibility */
        QString                 _failureString;     /** Reason(s) for the incompatibility */
    };

    /**
     * Construct with hardware specification \p type
     * @param type Hardware specification type
     */
    HardwareSpec(const Type& type);

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
     * Get the hardware component specification by \p name
     * @param name Name of the hardware component specification
     * @return Hardware component specification with the given name
     */
    template<class HardwareComponentSpecType>
    std::shared_ptr<HardwareComponentSpecType> getHardwareComponentSpec(const QString& name) const {
        static_assert(std::is_base_of_v<HardwareComponentSpec, HardwareComponentSpecType>, "HardwareComponentSpecType must derive from HardwareComponentSpec");

        for (const auto& hardwareComponentSpec : _hardwareComponentSpecs) {
            if (hardwareComponentSpec->getTitle() == name)
                return std::dynamic_pointer_cast<HardwareComponentSpecType>(hardwareComponentSpec);
        }

        return {};
    }

    /**
     * Get the hardware component specifications
     * @param initializedOnly Whether to only return initialized hardware component specifications
     * @return Hardware component specifications
     */
    HardwareComponentSpecs getHardwareComponentSpecs(bool initializedOnly = true) const;

public: // Conditional

    /**
     * Get whether the hardware specification is equal to the \p other hardware specification
     * @param other Other hardware specification to compare with
     * @return Boolean determining whether the hardware specification is equal to the \p other hardware specification
     */
    bool operator==(const HardwareSpec& other) const {
        if (_hardwareComponentSpecs.size() != other._hardwareComponentSpecs.size())
            return false;

        for (std::size_t i = 0; i < _hardwareComponentSpecs.size(); ++i) {
            const auto& lhs = _hardwareComponentSpecs[i];
            const auto& rhs = other._hardwareComponentSpecs[i];

            if (!lhs->isInitialized() || !rhs->isInitialized())
                return false;

            if (!lhs->equals(*rhs))
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

        conditions.reserve(_hardwareComponentSpecs.size());

        for (std::size_t i = 0; i < _hardwareComponentSpecs.size() && i < other._hardwareComponentSpecs.size(); ++i) {
            const auto& lhs = _hardwareComponentSpecs[i];
            const auto& rhs = other._hardwareComponentSpecs[i];

            if (lhs->isInitialized() && rhs->isInitialized())
                conditions.push_back(lhs->lessThan(*rhs));
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
     * @return Boolean determining whether the hardware specification meets the \p required hardware specification
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

    /**
     * Get system compatibility with the minimum and recommended hardware specifications
     * @param minimumHardwareSpec Minimum hardware specification
     * @param recommendedHardwareSpec Recommended hardware specification
     * @return System compatibility info with the minimum and recommended hardware specifications
     */
    static SystemCompatibilityInfo getSystemCompatibility(const HardwareSpec& minimumHardwareSpec, const HardwareSpec& recommendedHardwareSpec);

private:
    Type                    _type;                      /** Hardware specification type */
    HardwareComponentSpecs  _hardwareComponentSpecs;    /** Hardware component specifications */

    static HardwareSpec systemHardwareSpec; /** System hardware specification */
};

}
