#pragma once

#include "Vector2f.h"

namespace hdps
{
    class Selection
    {
    public:
        Selection();
        Selection(Vector2f start, Vector2f end);
        ~Selection();

        void set(Vector2f start, Vector2f end);

        void setStart(Vector2f start);
        void setEnd(Vector2f end);

        Vector2f getStart() const;
        Vector2f getEnd() const;
        Vector2f getCenter() const;

        float getLeft() const;
        float getRight() const;
        float getBottom() const;
        float getTop() const;
        Vector2f topLeft() const;
        Vector2f bottomLeft() const;
        Vector2f bottomRight() const;
        Vector2f topRight() const;

        bool contains(Vector2f point) const;

    private:
        void updateProperties();

        Vector2f _start, _end;

        float _left, _right, _bottom, _top;
    };

} // namespace hdps
