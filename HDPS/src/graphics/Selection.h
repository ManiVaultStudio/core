#ifndef SELECTION_H
#define SELECTION_H

#include "Vector2f.h"

namespace hdps
{
    class Selection
    {
    public:
        Selection()
        {
            set(Vector2f(0, 0), Vector2f(0, 0));
        }

        Selection(Vector2f start, Vector2f end)
        {
            set(start, end);
        }

        ~Selection() { }

        void set(Vector2f start, Vector2f end)
        {
            this->start = start;
            this->end = end;
        }

        void setStart(Vector2f start)
        {
            this->start = start;
        }

        void setEnd(Vector2f end)
        {
            this->end = end;
        }

        Vector2f getStart() const
        {
            return start;
        }

        Vector2f getEnd() const
        {
            return end;
        }

        Vector2f topLeft() const
        {
            return Vector2f(start.x < end.x ? start.x : end.x, start.y > end.y ? start.y : end.y);
        }

        Vector2f bottomRight() const
        {
            return Vector2f(start.x > end.x ? start.x : end.x, start.y < end.y ? start.y : end.y);
        }

        bool contains(Vector2f point) const
        {
            Vector2f tl = topLeft();
            Vector2f br = bottomRight();

            return point.x >= tl.x && point.x <= br.x && point.y >= br.y && point.y <= tl.y;
        }

    private:
        Vector2f start, end;
    };

} // namespace hdps

#endif // SELECTION_H
