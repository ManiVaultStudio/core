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
        void set(float x, float y, float z);

        /**
        * Adds the given vector to this vector.
        * @param v the vector to be added
        */
        void add(Vector3f v);

        /**
        * Subtracts the given vector from this vector.
        * @param v the vector to be subtracted
        */
        void sub(Vector3f v);

        /**
        * Returns the squared magnitude of this vector.
        * @return the squared magnitude of this vector
        */
        float sqrMagnitude();

        /**
        * Returns the magnitude of this vector.
        * @return the magnitude of this vector
        */
        float length();

        /* Operator overloads */
        bool operator==(const Vector3f& v) const;
        bool operator!=(const Vector3f& v) const;
        Vector3f& operator+=(const Vector3f& v);
        Vector3f& operator-=(const Vector3f& v);
        Vector3f& operator*=(const Vector3f& v);
        Vector3f& operator*=(float scale);
        Vector3f& operator/=(const Vector3f& v);
        Vector3f operator+(const Vector3f& v) const;
        Vector3f operator-(const Vector3f& v) const;
        Vector3f operator-() const;
        Vector3f operator*(const Vector3f& v) const;
        Vector3f operator*(float scale) const;
        Vector3f operator/(float divisor) const;

        float x, y, z;
    };

} // namespace hdps
