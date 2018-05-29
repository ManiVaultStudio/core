#pragma once

#include <cmath>

namespace hdps
{
    class Vector3f
    {
    public:
        Vector3f() : x(0), y(0), z(0) { }
        Vector3f(float x, float y, float z) : x(x), y(y), z(z) { }
        ~Vector3f() { }

        inline void set(float x, float y, float z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        inline void add(Vector3f v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
        }

        inline void sub(Vector3f v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
        }

        inline float sqrMagnitude()
        {
            return x * x + y * y + z * z;
        }

        inline float length()
        {
            return sqrt(sqrMagnitude());
        }

        /* Operator overloads */
        bool operator==(const Vector3f& v) const
        {
            return x == v.x && y == v.y && z == v.z;
        }

        bool operator!=(const Vector3f& v) const
        {
            return x != v.x || y != v.y || z != v.z;
        }

        Vector3f& operator+=(const Vector3f& v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }

        Vector3f& operator-=(const Vector3f& v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return *this;
        }

        Vector3f& operator*=(const Vector3f& v)
        {
            x *= v.x;
            y *= v.y;
            z *= v.z;
            return *this;
        }

        Vector3f& operator*=(float scale)
        {
            x *= scale;
            y *= scale;
            z *= scale;
            return *this;
        }

        Vector3f& operator/=(const Vector3f& v)
        {
            x /= v.x;
            y /= v.y;
            z /= v.z;
            return *this;
        }

        Vector3f operator+(const Vector3f& v) const
        {
            return Vector3f(x + v.x, y + v.y, z + v.z);
        }

        Vector3f operator-(const Vector3f& v) const
        {
            return Vector3f(x - v.x, y - v.y, z - v.z);
        }

        Vector3f operator-() const
        {
            return Vector3f(-x, -y, -z);
        }

        Vector3f operator*(const Vector3f& v) const
        {
            return Vector3f(x * v.x, y * v.y, z * v.z);
        }

        Vector3f operator*(float scale) const
        {
            return Vector3f(x * scale, y * scale, z * scale);
        }

        Vector3f operator/(float divisor) const
        {
            return Vector3f(x / divisor, y / divisor, z / divisor);
        }

        float x, y, z;
    };

} // namespace hdps
