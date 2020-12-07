#include "SelectionTool.h"

#include <QDebug>
#include <QEvent>
#include <QMouseEvent>

const QMap<QString, SelectionTool::Type> SelectionTool::types = {
    { "None", SelectionTool::Type::None },
    { "Rectangle", SelectionTool::Type::Rectangle },
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
    _brushRadius(DEFAULT_BRUSH_RADIUS),
    _mousePositions(),
    _mouseButtons()
{
    parent->installEventFilter(this);
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
                        setBrushRadius(_brushRadius - 5.0f);
                    else
                        setBrushRadius(_brushRadius + 5.0f);

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
            /*
            auto keyEvent = static_cast<QKeyEvent*>(event);

            switch (keyEvent->key())
            {
                case Qt::Key::Key_R:
                    setPixelSelectionType(SelectionType::Rectangle);
                    break;

                case Qt::Key::Key_B:
                    setPixelSelectionType(SelectionType::Brush);
                    break;

                case Qt::Key::Key_P:
                    setPixelSelectionType(SelectionType::Polygon);
                    break;

                case Qt::Key::Key_L:
                    setPixelSelectionType(SelectionType::Lasso);
                    break;

                case Qt::Key::Key_S:
                    setPixelSelectionType(SelectionType::Sample);
                    break;

                case Qt::Key::Key_A:
                {
                    selectAll();
                    break;
                }

                case Qt::Key::Key_D:
                {
                    selectNone();
                    break;
                }

                case Qt::Key::Key_I:
                {
                    invertSelection();
                    break;
                }

                case Qt::Key::Key_Z:
                {
                    zoomToSelection();
                    break;
                }

                case Qt::Key::Key_Shift:
                {
                    getPixelSelectionModifier(SelectionModifier::Add);
                    break;
                }

                case Qt::Key::Key_Control:
                {
                    getPixelSelectionModifier(SelectionModifier::Remove);
                    break;
                }

                case Qt::Key::Key_Escape:
                {
                    switch (_selectionType)
                    {
                        case SelectionType::None:
                        case SelectionType::Rectangle:
                        case SelectionType::Brush:
                            break;

                        case SelectionType::Lasso:
                        case SelectionType::Polygon:
                            _mousePositions.clear();
                            break;

                        default:
                            break;
                    }

                    break;
                }

                default:
                    break;
            }

            switch (keyEvent->key())
            {
                case Qt::Key::Key_R: {
                    affectedIds << index.siblingAtColumn(ult(Column::PixelSelectionType));
                    break;
                }

                case Qt::Key::Key_B: {
                    affectedIds << index.siblingAtColumn(ult(Column::BrushRadius));
                    affectedIds << index.siblingAtColumn(ult(Column::PixelSelectionType));
                    break;
                }

                case Qt::Key::Key_L: {
                    affectedIds << index.siblingAtColumn(ult(Column::PixelSelectionType));
                    break;
                }

                case Qt::Key::Key_P: {
                    affectedIds << index.siblingAtColumn(ult(Column::PixelSelectionType));
                    break;
                }

                case Qt::Key::Key_S: {
                    affectedIds << index.siblingAtColumn(ult(Column::PixelSelectionType));
                    break;
                }

                case Qt::Key::Key_A:
                case Qt::Key::Key_D:
                case Qt::Key::Key_I:
                    affectedIds << index.siblingAtColumn(ult(Layer::Column::Selection));
                    break;

                case Qt::Key::Key_Shift:
                case Qt::Key::Key_Control:
                    affectedIds << index.siblingAtColumn(ult(Column::PixelSelectionModifier));
                    break;

                default:
                    break;
            }
            */

            break;
        }

        case QEvent::KeyRelease:
        {
            /*
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
                    getPixelSelectionModifier(SelectionModifier::Replace);

                    affectedIds << index.siblingAtColumn(ult(Column::PixelSelectionModifier));
                    break;
                }

                default:
                    break;
            }
            */

            break;
        }

        default:
            break;
    }

    return QObject::eventFilter(target, event);
}