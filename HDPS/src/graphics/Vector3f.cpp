#include "Vector3f.h"

#include <cmath>

namespace hdps
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
