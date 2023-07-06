// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Vector2f.h"

#include <cmath>

namespace hdps
{
    void Vector2f::set(float x, float y)
    {
        this->x = x;
        this->y = y;
    }

    void Vector2f::add(Vector2f v)
    {
        x += v.x;
        y += v.y;
    }

    void Vector2f::sub(Vector2f v)
    {
        x -= v.x;
        y -= v.y;
    }

    float Vector2f::length() const
    {
        return sqrt(sqrMagnitude());
    }

    std::string Vector2f::str() const
    {
        return std::string("(" + std::to_string(x) + " " + std::to_string(y) + ")");
    }

    /* Operator overloads */
    Vector2f& Vector2f::operator+=(const Vector2f& v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    Vector2f& Vector2f::operator-=(const Vector2f& v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    Vector2f& Vector2f::operator*=(const Vector2f& v)
    {
        x *= v.x;
        y *= v.y;
        return *this;
    }

    Vector2f& Vector2f::operator*=(float scale)
    {
        x *= scale;
        y *= scale;
        return *this;
    }

    Vector2f& Vector2f::operator/=(const Vector2f& v)
    {
        x /= v.x;
        y /= v.y;
        return *this;
    }


    static_assert(Vector2f{}.x == 0.0f, "Compile-time unit test");
    static_assert(Vector2f{}.y == 0.0f, "Compile-time unit test");
    static_assert(Vector2f{} == Vector2f{}, "Compile-time unit test");
    static_assert(! (Vector2f{} != Vector2f{}), "Compile-time unit test");
    static_assert(Vector2f(0.0f, 0.0f) == Vector2f{}, "Compile-time unit test");
    static_assert(Vector2f(0.5f, 0.5f) != Vector2f{}, "Compile-time unit test");
    static_assert(! (Vector2f(0.5f, 0.5f) == Vector2f{}), "Compile-time unit test");
    static_assert(Vector2f(0.5f, 0.0f).x == 0.5f, "Compile-time unit test");
    static_assert(Vector2f(0.0f, 0.5f).y == 0.5f, "Compile-time unit test");
    static_assert(Vector2f(0.5f, 1.0f).sqrMagnitude() == 1.25f, "Compile-time unit test");
    static_assert(Vector2f(0.5f, 1.5f) + Vector2f(1.0f, 2.0f) == Vector2f(1.5f, 3.5f), "Compile-time unit test");
    static_assert(Vector2f(1.0f, 2.0f) + 0.5f == Vector2f(1.5f, 2.5f), "Compile-time unit test");
    static_assert(Vector2f(3.0f, 4.0f) - Vector2f(1.5f, 0.5f) == Vector2f(1.5f, 3.5f), "Compile-time unit test");
    static_assert(Vector2f(2.0f, 3.0f) - 0.5f == Vector2f(1.5f, 2.5f), "Compile-time unit test");
    static_assert(-Vector2f(0.5f, -1.5f) == Vector2f(-0.5f, 1.5f), "Compile-time unit test");
    static_assert(Vector2f(2.0f, 3.0f) * Vector2f(1.5f, 0.5f) == Vector2f(3.0f, 1.5f), "Compile-time unit test");
    static_assert(Vector2f(2.0f, 3.0f) * 1.5f == Vector2f(3.0f, 4.5f), "Compile-time unit test");
    static_assert(Vector2f(1.0f, 2.0f) / 2.0f == Vector2f(0.5f, 1.0f), "Compile-time unit test");

}
