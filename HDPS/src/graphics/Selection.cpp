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
        _start = start;
        _end = end;

        updateProperties();
    }

    void Selection::setStart(Vector2f start)
    {
        set(start, _end);
    }

    void Selection::setEnd(Vector2f end)
    {
        set(_start, end);
    }

    Vector2f Selection::getStart() const
    {
        return _start;
    }

    Vector2f Selection::getEnd() const
    {
        return _end;
    }

    Vector2f Selection::getCenter() const
    {
        return Vector2f((_left + _right) / 2, (_bottom + _top) / 2);
    }

    float Selection::getLeft() const
    {
        return _left;
    }
    float Selection::getRight() const
    {
        return _right;
    }

    float Selection::getBottom() const
    {
        return _bottom;
    }

    float Selection::getTop() const
    {
        return _top;
    }

    Vector2f Selection::topLeft() const
    {
        return Vector2f(_left, _top);
    }

    Vector2f Selection::bottomLeft() const
    {
        return Vector2f(_left, _bottom);
    }

    Vector2f Selection::bottomRight() const
    {
        return Vector2f(_right, _bottom);
    }

    Vector2f Selection::topRight() const
    {
        return Vector2f(_right, _top);
    }

    bool Selection::contains(Vector2f point) const
    {
        Vector2f tl = topLeft();
        Vector2f br = bottomRight();

    void Selection::updateProperties()
    {
        _left = std::min(_start.x, _end.x);
        _right = std::max(_start.x, _end.x);
        _bottom = std::min(_start.y, _end.y);
        _top = std::max(_start.y, _end.y);
        return point.x >= tl.x && point.x <= br.x && point.y >= br.y && point.y <= tl.y;
    }

} // namespace hdps
