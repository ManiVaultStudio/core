#include "Selection.h"

namespace hdps
{
    Selection::Selection()
    {
        set(Vector2f(0, 0), Vector2f(0, 0));
    }

    Selection::Selection(Vector2f start, Vector2f end)
    {
        set(start, end);
    }

    Selection::~Selection() { }

    void Selection::set(Vector2f start, Vector2f end)
    {
        this->start = start;
        this->end = end;
    }

    void Selection::setStart(Vector2f start)
    {
        this->start = start;
    }

    void Selection::setEnd(Vector2f end)
    {
        this->end = end;
    }

    Vector2f Selection::getStart() const
    {
        return start;
    }

    Vector2f Selection::getEnd() const
    {
        return end;
    }

    Vector2f Selection::getCenter() const
    {
        return Vector2f((start.x + end.x) / 2, (start.y + end.y) / 2);
    }

    Vector2f Selection::topLeft() const
    {
        return Vector2f(start.x < end.x ? start.x : end.x, start.y > end.y ? start.y : end.y);
    }

    Vector2f Selection::bottomLeft() const
    {
        return Vector2f(start.x < end.x ? start.x : end.x, start.y < end.y ? start.y : end.y);
    }

    Vector2f Selection::bottomRight() const
    {
        return Vector2f(start.x > end.x ? start.x : end.x, start.y < end.y ? start.y : end.y);
    }

    Vector2f Selection::topRight() const
    {
        return Vector2f(start.x > end.x ? start.x : end.x, start.y > end.y ? start.y : end.y);
    }

    bool Selection::contains(Vector2f point) const
    {
        Vector2f tl = topLeft();
        Vector2f br = bottomRight();

        return point.x >= tl.x && point.x <= br.x && point.y >= br.y && point.y <= tl.y;
    }

} // namespace hdps
