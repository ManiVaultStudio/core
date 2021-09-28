#pragma once

#include <QMap>

namespace hdps {

namespace util {

/** Pixel selection types enum */
enum class PixelSelectionType
{
    Rectangle,      /** Select pixels within a rectangle */
    Brush,          /** A brush is used the paint the pixel selection */
    Lasso,          /** A lasso tool is used to select pixels */
    Polygon,        /** Select pixels in the interior of a polygon */
    Sample          /** Probe one pixel at a time */
};

/** Pixel selection types alias */
using PixelSelectionTypes = QVector<PixelSelectionType>;

/** Default pixel selection types */
static const PixelSelectionTypes defaultPixelSelectionTypes = {
    PixelSelectionType::Rectangle,
    PixelSelectionType::Brush,
    PixelSelectionType::Polygon,
    PixelSelectionType::Lasso
};

/** Maps pixel selection type enum to name */
static const QMap<PixelSelectionType, QString> pixelSelectionTypes = {
    { PixelSelectionType::Rectangle, "Rectangle" },
    { PixelSelectionType::Brush, "Brush" },
    { PixelSelectionType::Lasso, "Lasso" },
    { PixelSelectionType::Polygon, "Polygon" },
    { PixelSelectionType::Sample, "Sample" }
};

/** Selection modifiers enum */
enum class PixelSelectionModifierType
{
    Replace,        /** Replace selection */
    Add,            /** Add to selection */
    Remove          /** Remove from selection */
};

/** Maps pixel selection modifier name to modifier enum and vice versa */
static const QMap<PixelSelectionModifierType, QString> pixelSelectionModifiers = {
    { PixelSelectionModifierType::Replace, "Replace" },
    { PixelSelectionModifierType::Add, "Add" },
    { PixelSelectionModifierType::Remove, "Remove" }
};

}
}
