// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QMap>
#include <QString>

namespace mv {

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
