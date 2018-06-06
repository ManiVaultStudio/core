#include "Vector2f.h"

#include <cmath>

namespace hdps
{
    Vector2f::Vector2f()
        : x(0), y(0)
    {

    }

    Vector2f::Vector2f(float x, float y)
        : x(x), y(y)
    {

    }

    Vector2f::~Vector2f()
    {

    }

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

    float Vector2f::sqrMagnitude() const
    {
        return x * x + y * y;
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
    bool Vector2f::operator==(const Vector2f& v) const
    {
        return x == v.x && y == v.y;
    }

    bool Vector2f::operator!=(const Vector2f& v) const
    {
        return x != v.x || y != v.y;
    }

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

    Vector2f Vector2f::operator+(const Vector2f& v) const
    {
        return Vector2f(x + v.x, y + v.y);
    }

    Vector2f Vector2f::operator+(float value) const
    {
        return Vector2f(x + value, y + value);
    }

    Vector2f Vector2f::operator-(const Vector2f& v) const
    {
        return Vector2f(x - v.x, y - v.y);
    }

    Vector2f Vector2f::operator-(float value) const
    {
        return Vector2f(x - value, y - value);
    }

    Vector2f Vector2f::operator-() const
    {
        return Vector2f(-x, -y);
    }

    Vector2f Vector2f::operator*(const Vector2f& v) const
    {
        return Vector2f(x * v.x, y * v.y);
    }

    Vector2f Vector2f::operator*(float scale) const
    {
        return Vector2f(x * scale, y * scale);
    }

    Vector2f Vector2f::operator/(float divisor) const
    {
        return Vector2f(x / divisor, y / divisor);
    }
}
