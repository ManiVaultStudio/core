// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Vector3f.h"

#include <QVariantMap>

#include <cmath>

namespace mv
{
    void Vector3f::set(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    void Vector3f::add(Vector3f v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
    }

    void Vector3f::sub(Vector3f v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
    }

    float Vector3f::length()
    {
        return sqrt(sqrMagnitude());
    }

    std::string Vector3f::str() const
    {
        return std::string("(" + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z) + ")");
    }

    QVariantMap Vector3f::toVariantMap() const
    {
        QVariantMap map;
        map["x"] = x;
        map["y"] = y;
        map["z"] = z;
        return map;
    }

    void Vector3f::fromVariantMap(const QVariantMap& map)
    {
        x = map["x"].toFloat();
        y = map["y"].toFloat();
        z = map["z"].toFloat();
    }

    /* Operator overloads */

    Vector3f& Vector3f::operator+=(const Vector3f& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    Vector3f& Vector3f::operator-=(const Vector3f& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    Vector3f& Vector3f::operator*=(const Vector3f& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }

    Vector3f& Vector3f::operator*=(float scale)
    {
        x *= scale;
        y *= scale;
        z *= scale;
        return *this;
    }

    Vector3f& Vector3f::operator/=(const Vector3f& v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    }

    std::ostream& operator<<(std::ostream& os, const Vector3f& v)
    {
        os << '(' << v.x << ", " << v.y << ", " << v.z << ')';
        return os;
    }

    mv::Vector3f min(mv::Vector3f a, mv::Vector3f b)
    {
        return Vector3f(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
    }

    mv::Vector3f max(mv::Vector3f a, mv::Vector3f b)
    {
        return Vector3f(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
    }

    static_assert(Vector3f{}.x == 0.0f, "Compile-time unit test");
    static_assert(Vector3f{}.y == 0.0f, "Compile-time unit test");
    static_assert(Vector3f{}.z == 0.0f, "Compile-time unit test");
    static_assert(Vector3f{} == Vector3f{}, "Compile-time unit test");
    static_assert(!(Vector3f{} != Vector3f{}), "Compile-time unit test");
    static_assert(Vector3f(0.0f, 0.0f, 0.0f) == Vector3f{}, "Compile-time unit test");
    static_assert(Vector3f(0.5f, 0.5f, 0.5f) != Vector3f{}, "Compile-time unit test");
    static_assert(!(Vector3f(0.5f, 0.5f, 0.5f) == Vector3f{}), "Compile-time unit test");
    static_assert(Vector3f(0.5f, 0.0f, 0.0f).x == 0.5f, "Compile-time unit test");
    static_assert(Vector3f(0.0f, 0.5f, 0.0f).y == 0.5f, "Compile-time unit test");
    static_assert(Vector3f(0.0f, 0.0f, 0.5f).z == 0.5f, "Compile-time unit test");
    static_assert(Vector3f(0.5f, 1.0f, 1.5f).sqrMagnitude() == 3.5f, "Compile-time unit test");
    static_assert(Vector3f(0.5f, 1.5f, 2.5) + Vector3f(1.0f, 2.0f, 3.0f) == Vector3f(1.5f, 3.5f, 5.5), "Compile-time unit test");
    static_assert(Vector3f(3.0f, 4.0f, 5.0f) - Vector3f(1.5f, 0.5f, -0.5f) == Vector3f(1.5f, 3.5f, 5.5f), "Compile-time unit test");
    static_assert(-Vector3f(0.5f, -1.5f, 2.5f) == Vector3f(-0.5f, 1.5f, -2.5f), "Compile-time unit test");
    static_assert(Vector3f(2.0f, 3.0f, 4.0f) * Vector3f(1.5f, 0.5f, -0.5f) == Vector3f(3.0f, 1.5f, -2.0), "Compile-time unit test");
    static_assert(Vector3f(2.0f, 3.0f, 4.0f) * 1.5f == Vector3f(3.0f, 4.5f, 6.0f), "Compile-time unit test");
    static_assert(Vector3f(1.0f, 2.0f, 3.0f) / 2.0f == Vector3f(0.5f, 1.0f, 1.5f), "Compile-time unit test");

}
