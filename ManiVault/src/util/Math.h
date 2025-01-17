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

CORE_EXPORT float lerp(float v0, float v1, float t);

inline bool arePointsEqual(const QPointF& point1, const QPointF& point2, qreal epsilon = 1e-5) {
    return qAbs(point1.x() - point2.x()) < epsilon && qAbs(point1.y() - point2.y()) < epsilon;
}

inline bool areSizesEqual(const QSizeF& size1, const QSizeF& size2, qreal epsilon = 1e-5) {
    return qAbs(size1.width() - size2.width()) < epsilon && qAbs(size1.height() - size2.height()) < epsilon;
}

inline bool areRectanglesEqual(const QRectF& rect1, const QRectF& rect2, qreal epsilon = 1e-5) {
    return arePointsEqual(rect1.topLeft(), rect2.topLeft()) && areSizesEqual(rect1.size(), rect2.size());
}

}
