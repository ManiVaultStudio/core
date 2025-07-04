// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QString>

namespace mv::util
{

CORE_EXPORT QString loadFileContents(const QString& path);

CORE_EXPORT class HardwareSpecs
{
public:

    /** Display resolution hardware spec */
    struct DisplayResolution
    {
        std::int32_t    _horizontal;    /** Horizontal resolution in pixels */
        std::int32_t    _vertical;      /** Vertical resolution in pixels */

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

        void fromSystem();
    };

private:
    DisplayResolution   _displayResolution;     /** Display resolution of the system */
};

}