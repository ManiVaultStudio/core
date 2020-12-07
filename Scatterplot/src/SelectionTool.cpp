#include "SelectionTool.h"

#include <QDebug>
#include <QEvent>
#include <QMouseEvent>

const QMap<QString, SelectionTool::Type> SelectionTool::types = {
    { "None", SelectionTool::Type::None },
    { "Rectangle", SelectionTool::Type::Rectangle },
    { "Circle", SelectionTool::Type::Circle },
    { "Brush", SelectionTool::Type::Brush },
    { "Lasso", SelectionTool::Type::Lasso },
    { "Polygon", SelectionTool::Type::Polygon },
};

const QMap<QString, SelectionTool::Modifier> SelectionTool::modifiers = {
    { "Replace", SelectionTool::Modifier::Replace },
    { "Add", SelectionTool::Modifier::Add },
    { "Remove", SelectionTool::Modifier::Remove },
    { "All", SelectionTool::Modifier::All },
    { "None", SelectionTool::Modifier::None },
    { "Invert", SelectionTool::Modifier::Invert }
};

SelectionTool::SelectionTool(QObject* parent) :
    QObject(parent),
    _type(Type::Rectangle),
    _modifier(Modifier::Replace),
    _radius(DEFAULT_RADIUS),
    _mousePositions(),
    _mouseButtons()
{
    parent->installEventFilter(this);
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

    _radius = radius;

    emit radiusChanged(_radius);
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
                case Type::None:
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
                case Type::None:
                    break;

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
                case Type::None:
                    break;

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
                case Type::None:
                case Type::Rectangle:
                    break;

                case Type::Brush:
                {
                    if (wheelEvent->delta() < 0)
                        setRadius(_radius - 5.0f);
                    else
                        setRadius(_radius + 5.0f);

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

        case QEvent::KeyPress:
        {
            
            auto keyEvent = static_cast<QKeyEvent*>(event);

            switch (keyEvent->key())
            {
                case Qt::Key::Key_R:
                {
                    setType(Type::Rectangle);
                    break;
                }

                case Qt::Key::Key_C:
                {
                    setType(Type::Circle);
                    break;
                }

                case Qt::Key::Key_B:
                {
                    setType(Type::Brush);
                    break;
                }

                case Qt::Key::Key_P:
                {
                    setType(Type::Polygon);
                    break;
                }

                case Qt::Key::Key_L:
                {
                    setType(Type::Lasso);
                    break;
                }

                case Qt::Key::Key_A:
                {
                    //selectAll();
                    break;
                }

                case Qt::Key::Key_D:
                {
                    //selectNone();
                    break;
                }

                case Qt::Key::Key_I:
                {
                    //invertSelection();
                    break;
                }

                case Qt::Key::Key_Shift:
                {
                    setModifier(Modifier::Add);
                    break;
                }

                case Qt::Key::Key_Control:
                {
                    setModifier(Modifier::Remove);
                    break;
                }

                case Qt::Key::Key_Escape:
                {
                    switch (_type)
                    {
                        case Type::None:
                        case Type::Rectangle:
                        case Type::Brush:
                            break;

                        case Type::Lasso:
                        case Type::Polygon:
                        {
                            _mousePositions.clear();
                            break;
                        }

                        default:
                            break;
                    }

                    break;
                }

                default:
                    break;
            }

            break;
        }

        case QEvent::KeyRelease:
        {
            auto keyEvent = static_cast<QKeyEvent*>(event);

            switch (keyEvent->key())
            {
                case Qt::Key::Key_R:
                case Qt::Key::Key_B:
                case Qt::Key::Key_L:
                case Qt::Key::Key_P:
                case Qt::Key::Key_A:
                case Qt::Key::Key_D:
                case Qt::Key::Key_I:
                case Qt::Key::Key_Z:
                    break;

                case Qt::Key::Key_Shift:
                case Qt::Key::Key_Control:
                {
                    setModifier(Modifier::Replace);
                    break;
                }

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