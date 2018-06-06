#include "Vector3f.h"

#include <cmath>

namespace hdps
{
    Vector3f::Vector3f()
        : x(0), y(0), z(0)
    {

    }

    Vector3f::Vector3f(float x, float y, float z)
        : x(x), y(y), z(z)
    {

    }

    Vector3f::~Vector3f()
    {

    }

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

    float Vector3f::sqrMagnitude()
    {
        return x * x + y * y + z * z;
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
    bool Vector3f::operator==(const Vector3f& v) const
    {
        return x == v.x && y == v.y && z == v.z;
    }

    bool Vector3f::operator!=(const Vector3f& v) const
    {
        return x != v.x || y != v.y || z != v.z;
    }

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

    Vector3f Vector3f::operator+(const Vector3f& v) const
    {
        return Vector3f(x + v.x, y + v.y, z + v.z);
    }

    Vector3f Vector3f::operator-(const Vector3f& v) const
    {
        return Vector3f(x - v.x, y - v.y, z - v.z);
    }

    Vector3f Vector3f::operator-() const
    {
        return Vector3f(-x, -y, -z);
    }

    Vector3f Vector3f::operator*(const Vector3f& v) const
    {
        return Vector3f(x * v.x, y * v.y, z * v.z);
    }

    Vector3f Vector3f::operator*(float scale) const
    {
        return Vector3f(x * scale, y * scale, z * scale);
    }

    Vector3f Vector3f::operator/(float divisor) const
    {
        return Vector3f(x / divisor, y / divisor, z / divisor);
    }
}
