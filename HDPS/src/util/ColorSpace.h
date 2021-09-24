#pragma once

#include <QMap>

namespace util {

/**
 * Color space
 * Defines color spaces
 */
enum class ColorSpace
{
    Mono,   /** Single channel */
    Duo,    /** Double channel */
    RGB,    /** Red Green Blue */
    HSL,    /** Hue Saturation Lightness */
    LAB     /**  */
};

/** Maps color space enum to name */
static const QMap<ColorSpace, QString> colorSpaces = {
    { ColorSpace::Mono, "Mono" },
    { ColorSpace::Duo, "Duo" },
    { ColorSpace::RGB, "RGB" },
    { ColorSpace::HSL, "HSL" },
    { ColorSpace::LAB, "LAB" }
};

}