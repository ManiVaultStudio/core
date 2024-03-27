// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Bounds.h"

#include <limits>

namespace mv
{
    const Bounds Bounds::Max = Bounds (
        std::numeric_limits<float>::infinity(),
        -std::numeric_limits<float>::infinity(),
        std::numeric_limits<float>::infinity(),
        -std::numeric_limits<float>::infinity()
    );

    Bounds::Bounds() :
        Bounds(-1, 1, -1, 1)
    { }

    Bounds::Bounds(float left, float right, float bottom, float top) :
        _left(left),
        _right(right),
        _bottom(bottom),
        _top(top)
    { }

    Bounds::Bounds(const Bounds& b) :
        _left(b._left),
        _right(b._right),
        _bottom(b._bottom),
        _top(b._top)
    { }

    void Bounds::setBounds(float left, float right, float bottom, float top)
    {
        _left = left;
        _right = right;
        _bottom = bottom;
        _top = top;
    }

    void Bounds::ensureMinimumSize(float width, float height)
    {
        Vector2f center = getCenter();

        if (getWidth() < width)
        {
            _left = center.x - width / 2;
            _right = center.y + width / 2;
        }
        if (getHeight() < height)
        {
            _bottom = center.y - height / 2;
            _top = center.y + height / 2;
        }
    }

    void Bounds::moveToOrigin()
    {
        Vector2f center = getCenter();
        _left -= center.x;
        _right -= center.x;
        _bottom -= center.y;
        _top -= center.y;
    }

    void Bounds::makeSquare()
    {
        Vector2f center = getCenter();
        float halfSize = getWidth() > getHeight() ? getWidth() / 2 : getHeight() / 2;

        _left = center.x - halfSize;
        _right = center.x + halfSize;
        _bottom = center.y - halfSize;
        _top = center.y + halfSize;
    }

    void Bounds::expand(float fraction)
    {
        float widthOffset = (getWidth() * fraction) / 2;
        float heightOffset = (getHeight() * fraction) / 2;

        _left -= widthOffset;
        _right += widthOffset;
        _bottom -= heightOffset;
        _top += heightOffset;
    }
}
