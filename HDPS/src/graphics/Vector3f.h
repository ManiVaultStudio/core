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

        /**
        * Sets the x, y and z components of this vector to the given parameters.
        * @param x the x-component of the vector
        * @param y the y-component of the vector
        * @param z the z-component of the vector
        */
        inline void set(float x, float y, float z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        /**
        * Adds the given vector to this vector.
        * @param v the vector to be added
        */
        inline void add(Vector3f v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
        }

        /**
        * Subtracts the given vector from this vector.
        * @param v the vector to be subtracted
        */
        inline void sub(Vector3f v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
        }

        /**
        * Returns the squared magnitude of this vector.
        * @return the squared magnitude of this vector
        */
        inline float sqrMagnitude()
        {
            return x * x + y * y + z * z;
        }

        /**
        * Returns the magnitude of this vector.
        * @return the magnitude of this vector
        */
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
