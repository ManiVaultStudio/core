// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QMap>

namespace hdps {

namespace util {

/** Color space types enum */
enum class ColorSpaceType
{
    Mono,   /** Single channel */
    Duo,    /** Double channel */
    RGB,    /** Red Green Blue */
    HSL,    /** Hue Saturation Lightness */
    LAB     /**  */
};

/** Maps color space type enum to name */
static const QMap<ColorSpaceType, QString> colorSpaces = {
    { ColorSpaceType::Mono, "Mono" },
    { ColorSpaceType::Duo, "Duo" },
    { ColorSpaceType::RGB, "RGB" },
    { ColorSpaceType::HSL, "HSL" },
    { ColorSpaceType::LAB, "LAB" }
};

}
}
