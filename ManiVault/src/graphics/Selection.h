// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "Vector2f.h"

namespace mv
{
    class CORE_EXPORT Selection
    {
    public:
        // C++ Rule of Zero: Implicitly defined destructor and copy member functions are fine for this class.

        Selection() = default;

        Selection(Vector2f start, Vector2f end);

        void set(Vector2f start, Vector2f end);

        void setStart(Vector2f start);
        void setEnd(Vector2f end);

        constexpr Vector2f getStart() const
        {
            return _start;
        }

        constexpr Vector2f getEnd() const
        {
            return _end;
        }

        constexpr Vector2f getCenter() const
        {
            return Vector2f((_left + _right) / 2, (_bottom + _top) / 2);
        }

        constexpr float getLeft() const
        {
            return _left;
        }

        constexpr float getRight() const
        {
            return _right;
        }

        constexpr float getBottom() const
        {
            return _bottom;
        }

        constexpr float getTop() const
        {
            return _top;
        }

        constexpr Vector2f topLeft() const
        {
            return Vector2f(_left, _top);
        }

        constexpr Vector2f bottomLeft() const
        {
            return Vector2f(_left, _bottom);
        }

        constexpr Vector2f bottomRight() const
        {
            return Vector2f(_right, _bottom);
        }

        constexpr Vector2f topRight() const
        {
            return Vector2f(_right, _top);
        }

        constexpr bool contains(Vector2f point) const
        {
            return point.x >= _left && point.x <= _right && point.y >= _bottom && point.y <= _top;
        }

    private:
        void updateProperties();

        Vector2f _start, _end;

        float _left{}, _right{}, _bottom{}, _top{};
    };

} // namespace mv
