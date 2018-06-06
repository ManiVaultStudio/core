#pragma once

#include <cmath>
#include <string>

namespace hdps
{
    class Vector2f
    {
    public:
        Vector2f() : x(0), y(0) { }
        Vector2f(float x, float y) : x(x), y(y) { }
        ~Vector2f() { }

        /**
        * Sets the x and y components of this vector to the given parameters.
        * @param x the x-component of the vector
        * @param y the y-component of the vector
        */
        inline void set(float x, float y)
        {
            this->x = x;
            this->y = y;
        }

        /**
        * Adds the given vector to this vector.
        * @param v the vector to be added
        */
        inline void add(Vector2f v)
        {
            x += v.x;
            y += v.y;
        }

        /**
        * Subtracts the given vector from this vector.
        * @param v the vector to be subtracted
        */
        inline void sub(Vector2f v)
        {
            x -= v.x;
            y -= v.y;
        }

        /**
        * Returns the squared magnitude of this vector.
        * @return the squared magnitude of this vector
        */
        inline float sqrMagnitude() const
        {
            return x * x + y * y;
        }

        /**
        * Returns the magnitude of this vector.
        * @return the magnitude of this vector
        */
        inline float length() const
        {
            return sqrt(sqrMagnitude());
        }

        /**
        * Returns a string representation of this vector.
        * @return the string representation
        */
        inline std::string str() const
        {
            return std::string("(" + std::to_string(x) + " " + std::to_string(y) + ")");
        }

        /* Operator overloads */
        bool operator==(const Vector2f& v) const
        {
            return x == v.x && y == v.y;
        }

        bool operator!=(const Vector2f& v) const
        {
            return x != v.x || y != v.y;
        }

        Vector2f& operator+=(const Vector2f& v)
        {
            x += v.x;
            y += v.y;
            return *this;
        }

        Vector2f& operator-=(const Vector2f& v)
        {
            x -= v.x;
            y -= v.y;
            return *this;
        }

        Vector2f& operator*=(const Vector2f& v)
        {
            x *= v.x;
            y *= v.y;
            return *this;
        }

        Vector2f& operator*=(float scale)
        {
            x *= scale;
            y *= scale;
            return *this;
        }

        Vector2f& operator/=(const Vector2f& v)
        {
            x /= v.x;
            y /= v.y;
            return *this;
        }

        Vector2f operator+(const Vector2f& v) const
        {
            return Vector2f(x + v.x, y + v.y);
        }

        Vector2f operator+(float value) const
        {
            return Vector2f(x + value, y + value);
        }

        Vector2f operator-(const Vector2f& v) const
        {
            return Vector2f(x - v.x, y - v.y);
        }

        Vector2f operator-(float value) const
        {
            return Vector2f(x - value, y - value);
        }

        Vector2f operator-() const
        {
            return Vector2f(-x, -y);
        }

        Vector2f operator*(const Vector2f& v) const
        {
            return Vector2f(x * v.x, y * v.y);
        }

        Vector2f operator*(float scale) const
        {
            return Vector2f(x * scale, y * scale);
        }

        Vector2f operator/(float divisor) const
        {
            return Vector2f(x / divisor, y / divisor);
        }

        float x, y;
    };

} // namespace hdps
