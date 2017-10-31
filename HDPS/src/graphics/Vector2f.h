#ifndef VECTOR2F_H
#define VECTOR2F_H

#include <cmath>

namespace hdps
{
    class Vector2f
    {
    public:
        Vector2f() : x(0), y(0) { }
        Vector2f(float x, float y) : x(x), y(y) { }
        ~Vector2f();

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

        inline float sqrMagnitude()
        {
            return x * x + y * y;
        }

        inline float length()
        {
            return sqrt(sqrMagnitude());
        }

        float x, y;
    };

} // namespace hdps

#endif // VECTOR2F_H
