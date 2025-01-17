// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Serializable.h"

#include <string>

namespace mv::util {

/**
 * Version class
 *
 * Based on semantic versioning (major, minor and patch version number)
 *
 * It alo has optional suffix
 */
class CORE_EXPORT Version final : public Serializable {
public:

    /**
     * Construct version with \p major, \p minor and \p patch version number and possibly \p suffix
     * @param major Major version number
     * @param minor Minor version number
     * @param patch Patch version number
     * @param suffix Suffix e.g. alpha, beta
     */
    Version(std::int32_t major = -1, std::int32_t minor = -1, std::int32_t patch = -1, const std::string& suffix = "");

    /**
     * Construct version with \p version string
     * @param version Version string
     */
    Version(const QString& version);

    /**
     * Initialize from \p major, \p minor and \p patch version number and possibly \p suffix
     * @param major Major version number
     * @param minor Minor version number
     * @param patch Patch version number
     * @param suffix Suffix e.g. alpha, beta
     */
    void initialize(std::int32_t major, std::int32_t minor, std::int32_t patch, const std::string& suffix = "");

    /**
     * Initialize from version string
     * @param version Version string
     */
    void initialize(const QString& version);

    /**
     * Get whether the version is valid
     * @return Boolean determining whether the version is valid
     */
    bool isValid() const;

    /**
     * Get context
     * @return Version context
     */
    std::string getContext() const;

    /**
     * Set version context to \p context
     * @param context Version context
     */
    void setContext(const std::string& context);

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

public: // Comparison operators

    /**
     * Determines whether this version is larger than \p rhs
     * @param rhs Right-hand-side version to compare with
     * @return Boolean determining whether this version is larger than \p rhs
     */
    bool operator > (const Version& rhs) const {
        if (getMajor() > rhs.getMajor())
            return true;

        if (getMajor() < rhs.getMajor())
            return false;

        if (getMinor() > rhs.getMinor())
            return true;

        if (getMinor() < rhs.getMinor())
            return false;

        return getPatch() > rhs.getPatch();
    }

    /**
     * Determines whether this version is smaller than \p rhs
     * @param rhs Right-hand-side version to compare with
     * @return Boolean determining whether this version is smaller than \p rhs
     */
    bool operator < (const Version& rhs) const {
        if (getMajor() < rhs.getMajor())
            return true;

        if (getMajor() > rhs.getMajor())
            return false;

        if (getMinor() < rhs.getMinor())
            return true;

        if (getMinor() > rhs.getMinor())
            return false;

        return getPatch() < rhs.getPatch();
    }

public: // Serialization

    /**
     * Load version from variant map
     * @param variantMap Variant map representation of the version
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save version to variant map
     * @return Variant map representation of the version
     */
    QVariantMap toVariantMap() const override;

private:
    std::string     _context;   /** Version context (to what the version applies) */
    std::int32_t    _major;     /** SEM version major */
    std::int32_t    _minor;     /** SEM version minor */
    std::int32_t    _patch;     /** SEM version patch */
    std::string     _suffix;    /** Version suffix */

    static const QString semanticVersioningRegex;
};

}
