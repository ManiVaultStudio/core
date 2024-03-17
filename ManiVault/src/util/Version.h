// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Serializable.h"

#include <string>

namespace mv::util {

/** Class for representing version with major-, minor- and patch version number plus an optional suffic */
class CORE_EXPORT Version final : public Serializable {
public:

    /**
     * Construct version with \p major and \p minor version number
     * @param major Major version number
     * @param minor Minor version number
     */
    Version(std::int32_t major, std::int32_t minor, std::int32_t patch, const std::string& suffix = "");

    /**
     * Get expanded version number, e.g. 40316 for 4.3.16
     * @return Expanded version number
     */
    std::int32_t getVersionNumber() const;

    /**
     * Get full version as string, including suffix: Major.Minor.PatchSuffix
     * @return version string
     */
    std::string getVersionString() const;

    /**
     * Get short version as string: Major.Minor
     * @return version string
     */
    std::string getShortVersionString() const;

    /**
     * Get major version number
     * @return Major version number
     */
    std::int32_t getMajor() const;

    /**
     * Set major version number to \p major
     * @param major Major version number
     */
    void setMajor(std::int32_t major);

    /**
     * Get minor version number
     * @return Minor version number
     */
    std::int32_t getMinor() const;

    /**
     * Set minor version number to \p minor
     * @param minor Minor version number
     */
    void setMinor(std::int32_t minor);

    /**
     * Get patch version number
     * @return Patch version number
     */
    std::int32_t getPatch() const;

    /**
     * Set patch version number to \p minor
     * @param patch Patch version number
     */
    void setPatch(std::int32_t patch);

    /**
     * Get minor version number
     * @return Minor version number
     */
    std::string getSuffix() const;

    /**
     * Set minor version number to \p minor
     * @param minor Minor version number
     */
    void setSuffix(const std::string& suffix);

public: // Serialization

    /**
     * Load version from variant map
     * @param Variant map representation of the version
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save version to variant map
     * @return Variant map representation of the version
     */
    QVariantMap toVariantMap() const override;

private:
    std::int32_t _major;
    std::int32_t _minor;
    std::int32_t _patch;
    std::string  _suffix;
};

}
