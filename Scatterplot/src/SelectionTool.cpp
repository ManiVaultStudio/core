#include "SelectionTool.h"
#include "ScatterplotPlugin.h"

#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>

const QMap<QString, SelectionTool::Type> SelectionTool::types = {
    { "Rectangle", SelectionTool::Type::Rectangle },
    { "Circle", SelectionTool::Type::Circle },
    { "Brush", SelectionTool::Type::Brush },
    { "Lasso", SelectionTool::Type::Lasso },
    { "Polygon", SelectionTool::Type::Polygon }
};

const QMap<QString, SelectionTool::Modifier> SelectionTool::modifiers = {
    { "Replace", SelectionTool::Modifier::Replace },
    { "Add", SelectionTool::Modifier::Add },
    { "Remove", SelectionTool::Modifier::Remove }
};

const QColor SelectionTool::COLOR_MAIN = Qt::black;
const QColor SelectionTool::COLOR_FILL = QColor(0, 0, 0, 100);
const QPen SelectionTool::PEN_FOREGROUND = QPen(COLOR_MAIN, 10.0f, Qt::SolidLine);
const QPen SelectionTool::PEN_BACKGROUND = QPen(COLOR_MAIN, 10.0f, Qt::DotLine);
const QPen SelectionTool::PEN_CONTROL_POINT = QPen(COLOR_MAIN, 10.0f, Qt::DotLine, Qt::RoundCap);

SelectionTool::SelectionTool(ScatterplotPlugin* scatterplotPlugin) :
    QObject(reinterpret_cast<QObject*>(scatterplotPlugin)),
    _scatterplotPlugin(scatterplotPlugin),
    _type(Type::Rectangle),
    _modifier(Modifier::Replace),
    _radius(RADIUS_DEFAULT),
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

bool SelectionTool::canSelect() const
{
    Q_ASSERT(_scatterplotPlugin != nullptr);

    return !_scatterplotPlugin->getCurrentDataset().isEmpty() && _scatterplotPlugin->getNumPoints() >= 0;
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
}

void SelectionTool::selectAll()
{
    Q_ASSERT(_scatterplotPlugin != nullptr);

    _scatterplotPlugin->selectAll();
}

void SelectionTool::clearSelection()
{
    Q_ASSERT(_scatterplotPlugin != nullptr);

    _scatterplotPlugin->clearSelection();
}

void SelectionTool::invertSelection()
{
    Q_ASSERT(_scatterplotPlugin != nullptr);

    _scatterplotPlugin->invertSelection();
}

bool SelectionTool::canSelectAll() const
{
    return _scatterplotPlugin->getNumPoints() == -1 ? false : _scatterplotPlugin->getNumSelectedPoints() != _scatterplotPlugin->getNumPoints();
}

bool SelectionTool::canClearSelection() const
{
    return _scatterplotPlugin->getNumPoints() == -1 ? false : _scatterplotPlugin->getNumSelectedPoints() >= 1;
}

bool SelectionTool::canInvertSelection() const
{
    return _scatterplotPlugin->getNumPoints() >= 0;
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

void SelectionTool::paint(QPainter* painter)
{
    painter->setFont(QFont("Font Awesome 5 Free Solid", 9));

    auto textRectangle = QRectF();

    auto fillBrush = QBrush(COLOR_FILL);

    QVector<QPoint> controlPoints;

    
    switch (_type)
    {
        case Type::Rectangle:
        {
            if (_mousePositions.size() == 2) {
                const auto topLeft = QPointF(std::min(_mousePositions.first().x(), _mousePositions.last().x()), std::min(_mousePositions.first().y(), _mousePositions.last().y()));
                const auto bottomRight = QPointF(std::max(_mousePositions.first().x(), _mousePositions.last().x()), std::max(_mousePositions.first().y(), _mousePositions.last().y()));
                const auto rectangle = QRectF(topLeft, bottomRight);

                qDebug() << rectangle;

                painter->setPen(QPen(Qt::red, 10.0f, Qt::SolidLine));
                painter->setBrush(Qt::NoBrush);
                painter->drawRect(rectangle);

                controlPoints << _mousePositions.first();
                controlPoints << _mousePositions.last();

                const auto size = 8.0f;
                const auto textCenter = rectangle.topRight() + QPoint(size, -size);

                textRectangle = QRectF(textCenter - QPointF(size, size), textCenter + QPointF(size, size));
            }

            break;
        }

        /*
        case SelectionType::Brush:
        {
            if (_mousePositions.size() >= 1) {
                const auto brushCenter = _mousePositions.last();

                painter->setPen(Qt::NoPen);
                painter->setBrush(toolColorForeground);

                painter->drawPoint(brushCenter);

                painter->setPen(_mouseButtons & Qt::LeftButton ? perimeterForegroundPen : perimeterBackgroundPen);

                painter->setBrush(Qt::NoBrush);
                painter->drawEllipse(QPointF(brushCenter), _brushRadius, _brushRadius);

                controlPoints << _mousePositions.last();

                const auto textAngle = 0.75f * M_PI;
                const auto size = 12.0f;
                const auto textCenter = brushCenter + (_brushRadius + size) * QPointF(sin(textAngle), cos(textAngle));

                textRectangle = QRectF(textCenter - QPointF(size, size), textCenter + QPointF(size, size));
            }

            break;
        }

        case SelectionType::Lasso:
        {
            if (_mousePositions.size() >= 2) {
                painter->setPen(perimeterForegroundPen);
                painter->drawPolyline(_mousePositions.constData(), _mousePositions.count());

                controlPoints << _mousePositions.first();
                controlPoints << _mousePositions.last();

                painter->setPen(perimeterBackgroundPen);
                painter->drawPolyline(controlPoints.constData(), controlPoints.count());

                const auto size = 8.0f;
                const auto textCenter = _mousePositions.first() - QPoint(size, size);

                textRectangle = QRectF(textCenter - QPointF(size, size), textCenter + QPointF(size, size));
            }

            break;
        }

        case SelectionType::Polygon:
        {
            if (_mousePositions.size() >= 2) {
                painter->setPen(QPen(QBrush(toolColorForeground), perimeterLineWidth));
                painter->drawPolyline(_mousePositions.constData(), _mousePositions.count());

                QVector<QPoint> connectingPoints;

                connectingPoints << _mousePositions.first();
                connectingPoints << _mousePositions.last();

                painter->setPen(QPen(QBrush(toolColorBackground), perimeterLineWidth, Qt::DashLine));

                painter->drawPolyline(connectingPoints.constData(), connectingPoints.count());

                controlPoints << _mousePositions;

                const auto size = 8.0f;
                const auto textCenter = _mousePositions.first() - QPoint(size, size);

                textRectangle = QRectF(textCenter - QPointF(size, size), textCenter + QPointF(size, size));
            }

            break;
        }

        case SelectionType::Sample:
        {
            if (_mousePositions.size() == 1) {
                const auto center = _mousePositions.last();
                const auto r1 = 3.0f;
                const auto r2 = 10.0f;

                painter->setPen(QPen(QBrush(toolColorForeground), perimeterLineWidth));

                for (int section = 0; section < 4; ++section) {
                    const auto rotation = section * M_PI_2;
                    const auto point = QPointF(qSin(rotation), qCos(rotation));

                    painter->drawLine(center + r1 * point, center + r2 * point);
                }
            }

            break;
        }
        */
        default:
            break;
    }

    
    painter->setPen(PEN_CONTROL_POINT);
    painter->drawPoints(controlPoints);
    /*
    switch (_selectionType)
    {
        case SelectionType::None:
            break;

        case SelectionType::Rectangle:
        case SelectionType::Brush:
        case SelectionType::Lasso:
        case SelectionType::Polygon:
        {
            switch (_selectionModifier)
            {
                case SelectionModifier::Replace:
                    break;

                case SelectionModifier::Add:
                {
                    painter->setPen(toolColorForeground);
                    painter->drawText(textRectangle, hdps::Application::getIconFont("FontAwesome").getIconCharacter("plus-circle"), QTextOption(Qt::AlignCenter));
                    break;
                }

                case SelectionModifier::Remove:
                {
                    painter->setPen(toolColorForeground);
                    painter->drawText(textRectangle, hdps::Application::getIconFont("FontAwesome").getIconCharacter("minus-circle"), QTextOption(Qt::AlignCenter));
                    break;
                }

                case SelectionModifier::All:
                case SelectionModifier::None:
                case SelectionModifier::Invert:
                    break;
                default:
                    break;
            }

            break;
        }

        default:
            break;
    }
    */
}