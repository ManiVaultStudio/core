#pragma once

#include <QMap>

namespace hdps {

namespace util {

/** Interpolation types enum */
enum class InterpolationType
{
    Bilinear,           /** Bilinear interpolation */
    NearestNeighbor,    /** Nearest neighbor interpolation */
};

/** Maps interpolation type enum to name */
static const QMap<InterpolationType, QString> interpolationTypes = {
    { InterpolationType::Bilinear, "Bilinear" },
    { InterpolationType::NearestNeighbor, "NearestNeighbor" }
};

}
}
