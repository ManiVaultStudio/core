// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Serializable.h"

#include <QPair>

namespace mv::util {

/** Class for representing version with major- and minor version number */
class Version final : public QPair<std::int32_t, std::int32_t>, public Serializable {
public:

    /**
     * Construct version with \p major and \p minor version number
     * @param major Major version number
     * @param minor Minor version number
     */
    Version(std::int32_t major, std::int32_t minor);

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
};

}
