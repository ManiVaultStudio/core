// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QString>

namespace mv::util
{

CORE_EXPORT class HardwareSpec
{
public:
    virtual ~HardwareSpec() = default;

protected: // Population methods

    /** Load the hardware spec from current system */
    virtual void fromSystem() = 0;

    /**
     * Load the hardware spec from \p variantMap
     * @param variantMap Variant map containing the hardware spec properties
     */
    virtual void fromVariantMap(const QVariantMap& variantMap) = 0;
};

/** Display resolution hardware spec */
CORE_EXPORT class DisplayResolution : public HardwareSpec
{
public:

    /** Default constructor */
    DisplayResolution();

    /**
     * Get whether the display resolution is smaller than \p other display resolution
     * @return Boolean determining whether the display resolution is smaller than \p other display resolution
     */
    bool operator<(const DisplayResolution& other) const {
        return _horizontal > other._horizontal && _vertical > other._vertical;
    }

    /**
     * Get whether the display resolution is larger than \p other display resolution
     * @return Boolean determining whether the display resolution is larger than \p other display resolution
     */
    bool operator>(const DisplayResolution& other) const {
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
    std::int32_t    _horizontal;    /** Horizontal resolution in pixels */
    std::int32_t    _vertical;      /** Vertical resolution in pixels */
};

/** Available RAM hardware spec */
CORE_EXPORT class AvailableRAM : public HardwareSpec
{
public:

    /** Default constructor */
    AvailableRAM();

    /**
     * Get the amount of available RAM in bytes
     * @return Number of bytes of available RAM
     */
    std::uint64_t getNumberOfBytes() const {
        return _numberOfBytes;
    }

    /**
     * Get whether the available RAM is smaller than \p other available RAM
     * @return Boolean determining whether the available RAM is smaller than the \p other available RAM
     */
    bool operator<(const AvailableRAM& other) const {
        return getNumberOfBytes() > other.getNumberOfBytes();
    }

    /**
     * Get whether the available RAM is smaller than \p other available RAM
     * @return Boolean determining whether the available RAM is smaller than the \p other available RAM
     */
    bool operator>(const AvailableRAM& other) const {
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