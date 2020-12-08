#include "SelectionTool.h"

#include <QDebug>
#include <QEvent>
#include <QMouseEvent>

const QMap<QString, SelectionTool::Type> SelectionTool::types = {
    { "Rectangle", SelectionTool::Type::Rectangle },
    { "Circle", SelectionTool::Type::Circle },
    { "Brush", SelectionTool::Type::Brush },
    { "Lasso", SelectionTool::Type::Lasso },
    { "Polygon", SelectionTool::Type::Polygon },
};

const QMap<QString, SelectionTool::Modifier> SelectionTool::modifiers = {
    { "Replace", SelectionTool::Modifier::Replace },
    { "Add", SelectionTool::Modifier::Add },
    { "Remove", SelectionTool::Modifier::Remove }
};

SelectionTool::SelectionTool(QObject* parent) :
    QObject(parent),
    _type(Type::Rectangle),
    _modifier(Modifier::Replace),
    _radius(RADIUS_DEFAULT),
    _numPoints(-1),
    _selectionSize(-1),
    _mousePositions(),
    _mouseButtons()
{
}

SelectionTool::Type SelectionTool::getType() const
{
    return _type;
}

void SelectionTool::setType(const Type& type)
{
    if (type == _type)
        return;

    _type = type;

    emit typeChanged(_type);
}

SelectionTool::Modifier SelectionTool::getModifier() const
{
    return _modifier;
}

void SelectionTool::setModifier(const Modifier& modifier)
{
    if (modifier == _modifier)
        return;

    _modifier = modifier;

    emit modifierChanged(_modifier);
}

float SelectionTool::getRadius() const
{
    return _radius;
}

void SelectionTool::setRadius(const float& radius)
{
    if (radius == _radius)
        return;

    _radius = std::clamp(radius, RADIUS_MIN, RADIUS_MAX);

    emit radiusChanged(_radius);
}

std::int32_t SelectionTool::getNumPoints() const
{
    return _numPoints;
}

void SelectionTool::setNumPoints(const std::int32_t& numPoints)
{
    if (numPoints == _numPoints)
        return;

    _numPoints = numPoints;

    emit selectionChanged(_selectionSize, _numPoints);
}

std::int32_t SelectionTool::getSelectionSize() const
{
    return _selectionSize;
}

void SelectionTool::setSelectionSize(const std::int32_t& selectionSize)
{
    if (selectionSize == _selectionSize)
        return;

    _selectionSize = selectionSize;

    emit selectionChanged(_selectionSize, _numPoints);
}

void SelectionTool::finish()
{
    _mousePositions.clear();
}

void SelectionTool::setChanged()
{
    emit typeChanged(_type);
    emit modifierChanged(_modifier);
    emit radiusChanged(_radius);
    emit selectionChanged(_selectionSize, _numPoints);
}

void SelectionTool::selectAll()
{

}

void SelectionTool::clearSelection()
{

}

void SelectionTool::invertSelection()
{

}

bool SelectionTool::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        // Prevent recursive paint events
        case QEvent::Paint:
            return false;

        case QEvent::MouseButtonPress:
        {
            auto mouseEvent = static_cast<QMouseEvent*>(event);

            _mouseButtons = mouseEvent->buttons();

            switch (_type)
            {
                case Type::Rectangle:
                {
                    _mousePositions.clear();
                    _mousePositions << mouseEvent->pos();
                    break;
                }

                case Type::Lasso:
                {
                    _mousePositions.clear();
                    _mousePositions << mouseEvent->pos();
                    break;
                }

                case Type::Polygon:
                {
                    switch (mouseEvent->button())
                    {
                        case Qt::LeftButton:
                        {
                            _mousePositions << mouseEvent->pos();
                            break;
                        }

                        case Qt::RightButton:
                        {
                            _mousePositions << mouseEvent->pos();
                            break;
                        }

                        default:
                            break;
                    }

                    break;
                }

                case Type::Brush:
                {
                    _mousePositions.clear();
                    _mousePositions << mouseEvent->pos();
                    break;
                }

                default:
                    break;
            }

            break;
        }

        case QEvent::MouseButtonRelease:
        {
            auto mouseEvent = static_cast<QMouseEvent*>(event);

            _mouseButtons = mouseEvent->buttons();

            switch (_type)
            {
                case Type::Rectangle:
                {
                    if (mouseEvent->button() == Qt::LeftButton) {
                        //publishSelection();
                        _mousePositions.clear();
                    }

                    break;
                }

                case Type::Lasso:
                {
                    if (mouseEvent->button() == Qt::LeftButton) {
                        //publishSelection();
                        _mousePositions.clear();
                    }

                    break;
                }

                case Type::Polygon:
                {
                    if (mouseEvent->button() == Qt::RightButton) {
                        _mousePositions.removeLast();
                        //publishSelection();
                        _mousePositions.clear();
                    }

                    break;
                }

                case Type::Brush:
                {
                    //publishSelection();
                    _mousePositions.clear();

                    break;
                }

                default:
                    break;
            }

            break;
        }

        case QEvent::MouseMove:
        {
            auto mouseEvent = static_cast<QMouseEvent*>(event);

            auto shouldComputePixelSelection = false;

            switch (_type)
            {
                case Type::Rectangle:
                {
                    if (mouseEvent->buttons() & Qt::LeftButton) {
                        if (_mousePositions.size() != 2)
                            _mousePositions << mouseEvent->pos();
                        else
                            _mousePositions.last() = mouseEvent->pos();

                        shouldComputePixelSelection = true;
                    }

                    break;
                }

                case Type::Circle:
                {
                    /*
                    if (mouseEvent->buttons() & Qt::LeftButton) {
                        if (_mousePositions.size() != 2)
                            _mousePositions << mouseEvent->pos();
                        else
                            _mousePositions.last() = mouseEvent->pos();

                        shouldComputePixelSelection = true;
                    }
                    */

                    break;
                }

                case Type::Brush:
                {
                    if (mouseEvent->buttons() & Qt::LeftButton) {
                        _mousePositions << mouseEvent->pos();
                        shouldComputePixelSelection = true;
                    }
                    else {
                        if (_mousePositions.isEmpty())
                            _mousePositions << mouseEvent->pos();

                        _mousePositions.last() = mouseEvent->pos();
                    }

                    break;
                }

                case Type::Lasso:
                {
                    if (mouseEvent->buttons() & Qt::LeftButton)
                        _mousePositions << mouseEvent->pos();

                    shouldComputePixelSelection = true;

                    break;
                }

                case Type::Polygon:
                {
                    if (_mousePositions.isEmpty())
                        _mousePositions << mouseEvent->pos();
                    else
                        _mousePositions.last() = mouseEvent->pos();

                    shouldComputePixelSelection = true;

                    break;
                }

                default:
                    break;
            }

            break;
        }

        case QEvent::Wheel:
        {
            auto wheelEvent = static_cast<QWheelEvent*>(event);

            switch (_type)
            {
                case Type::Rectangle:
                    break;

                case Type::Brush:
                {
                    if (wheelEvent->delta() < 0)
                        setRadius(_radius - RADIUS_DELTA);
                    else
                        setRadius(_radius + RADIUS_DELTA);

                    break;
                }

                case Type::Lasso:
                case Type::Polygon:
                    break;

                default:
                    break;
            }

            break;
        }

        default:
            break;
    }

    return QObject::eventFilter(target, event);
}