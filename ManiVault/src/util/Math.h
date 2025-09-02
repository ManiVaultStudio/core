// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QPointF>
#include <QRectF>
#include <QSize>

namespace mv::util
{

/**
 * Linearly interpolates between two values v0 and v1 by t
 * @param v0 Start value
 * @param v1 End value
 * @param t Interpolation factor in the range [0, 1]
 * @return Interpolated value
 */
CORE_EXPORT float lerp(float v0, float v1, float t);

/**
 * Checks whether two floating point numbers are almost equal
 * @param a Floating point number to compare
 * @param b Floating point number to compare with
 * @param eps Tolerance
 * @return Boolean determining whether the two floating point numbers are almost equal
 */
inline bool almostEqual(double a, double b, double eps = 1e-9) {
    return std::fabs(a - b) < eps;
}

/**
 * Checks whether two points are almost equal
 * @param point1 Point to compare
 * @param point2 Point to compare with
 * @param epsilon Tolerance
 * @return Boolean determining whether the two points are almost equal
 */
inline bool arePointsEqual(const QPointF& point1, const QPointF& point2, qreal epsilon = 1e-5) {
    return qAbs(point1.x() - point2.x()) < epsilon && qAbs(point1.y() - point2.y()) < epsilon;
}

/**
 * Checks whether two sizes are almost equal
 * @param size1 Size to compare
 * @param size2 Size to compare with
 * @param epsilon Tolerance
 * @return Boolean determining whether the two sizes are almost equal
 */
inline bool areSizesEqual(const QSizeF& size1, const QSizeF& size2, qreal epsilon = 1e-5) {
    return qAbs(size1.width() - size2.width()) < epsilon && qAbs(size1.height() - size2.height()) < epsilon;
}

/**
 * Checks whether two rectangles are almost equal
 * @param rect1 Rectangle to compare
 * @param rect2 Rectangle to compare with
 * @param epsilon Tolerance
 * @return Boolean determining whether the two rectangles are almost equal
 */
inline bool areRectanglesEqual(const QRectF& rect1, const QRectF& rect2, qreal epsilon = 1e-5) {
    return arePointsEqual(rect1.topLeft(), rect2.topLeft()) && areSizesEqual(rect1.size(), rect2.size());
}

}
