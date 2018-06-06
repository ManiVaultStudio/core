#pragma once

#include <string>

namespace hdps
{
    class Vector2f
    {
    public:
        Vector2f();
        Vector2f(float x, float y);
        ~Vector2f();

        /**
        * Sets the x and y components of this vector to the given parameters.
        * @param x the x-component of the vector
        * @param y the y-component of the vector
        */
        void set(float x, float y);

        /**
        * Adds the given vector to this vector.
        * @param v the vector to be added
        */
        void add(Vector2f v);

        /**
        * Subtracts the given vector from this vector.
        * @param v the vector to be subtracted
        */
        void sub(Vector2f v);

        /**
        * Returns the squared magnitude of this vector.
        * @return the squared magnitude of this vector
        */
        float sqrMagnitude() const;

        /**
        * Returns the magnitude of this vector.
        * @return the magnitude of this vector
        */
        float length() const;

        /**
        * Returns a string representation of this vector.
        * @return the string representation
        */
        std::string str() const;

        /* Operator overloads */
        bool operator==(const Vector2f& v) const;
        bool operator!=(const Vector2f& v) const;
        Vector2f& operator+=(const Vector2f& v);
        Vector2f& operator-=(const Vector2f& v);
        Vector2f& operator*=(const Vector2f& v);
        Vector2f& operator*=(float scale);
        Vector2f& operator/=(const Vector2f& v);
        Vector2f operator+(const Vector2f& v) const;
        Vector2f operator+(float value) const;
        Vector2f operator-(const Vector2f& v) const;
        Vector2f operator-(float value) const;
        Vector2f operator-() const;
        Vector2f operator*(const Vector2f& v) const;
        Vector2f operator*(float scale) const;
        Vector2f operator/(float divisor) const;

        float x, y;
    };

} // namespace hdps
