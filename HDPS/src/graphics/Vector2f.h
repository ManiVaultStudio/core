#ifndef VECTOR2F_H
#define VECTOR2F_H

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

        inline void set(float x, float y)
        {
            this->x = x;
            this->y = y;
        }

        inline void add(Vector2f v)
        {
            x += v.x;
            y += v.y;
        }

        inline void sub(Vector2f v)
        {
            x -= v.x;
            y -= v.y;
        }

        inline float sqrMagnitude() const
        {
            return x * x + y * y;
        }

        inline float length() const
        {
            return sqrt(sqrMagnitude());
        }

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

#endif // VECTOR2F_H
