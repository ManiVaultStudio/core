// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Selection.h"

#include <algorithm>

namespace mv
{
    Selection::Selection(Vector2f start, Vector2f end)
    {
        set(start, end);
    }

    void Selection::set(Vector2f start, Vector2f end)
    {
        _start = start;
        _end = end;

        updateProperties();
    }

    void Selection::setStart(Vector2f start)
    {
        set(start, _end);
    }

    void Selection::setEnd(Vector2f end)
    {
        set(_start, end);
    }

    void Selection::updateProperties()
    {
        _left = std::min(_start.x, _end.x);
        _right = std::max(_start.x, _end.x);
        _bottom = std::min(_start.y, _end.y);
        _top = std::max(_start.y, _end.y);
    }


    static_assert(Selection{}.getStart() == Vector2f{}, "Compile-time unit test");
    static_assert(Selection{}.getEnd() == Vector2f{}, "Compile-time unit test");
    static_assert(Selection{}.getCenter() == Vector2f{}, "Compile-time unit test");
    static_assert(Selection{}.getLeft() == 0.0f, "Compile-time unit test");
    static_assert(Selection{}.getRight() == 0.0f, "Compile-time unit test");
    static_assert(Selection{}.getBottom() == 0.0f, "Compile-time unit test");
    static_assert(Selection{}.getTop() == 0.0f, "Compile-time unit test");
    static_assert(Selection{}.topLeft() == Vector2f{}, "Compile-time unit test");
    static_assert(Selection{}.bottomLeft() == Vector2f{}, "Compile-time unit test");
    static_assert(Selection{}.bottomRight() == Vector2f{}, "Compile-time unit test");
    static_assert(Selection{}.topRight() == Vector2f{}, "Compile-time unit test");
    static_assert(Selection{}.contains(Vector2f{}), "Compile-time unit test");

} // namespace mv
