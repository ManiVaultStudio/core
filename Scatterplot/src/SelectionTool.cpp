#include "SelectionTool.h"
#include "ScatterplotPlugin.h"
#include "Application.h"

#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QtMath>

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

const QColor SelectionTool::COLOR_MAIN      = Qt::black;
const QColor SelectionTool::COLOR_FILL      = QColor(70, 70, 70, 100);
const QBrush SelectionTool::AREA_BRUSH      = QBrush(COLOR_FILL);
const QPen SelectionTool::PEN_LINE_FG       = QPen(COLOR_MAIN, 1.5f, Qt::SolidLine);
const QPen SelectionTool::PEN_LINE_BG       = QPen(QColor(COLOR_MAIN.red(), COLOR_MAIN.green(), COLOR_MAIN.blue(), 140), 1.5f, Qt::DotLine);
const QPen SelectionTool::PEN_CP            = QPen(COLOR_MAIN, 5.0f, Qt::SolidLine, Qt::RoundCap);

SelectionTool::SelectionTool(ScatterplotPlugin* scatterplotPlugin) :
    QObject(reinterpret_cast<QObject*>(scatterplotPlugin)),
    _scatterplotPlugin(scatterplotPlugin),
    _type(Type::Rectangle),
    _modifier(Modifier::Replace),
    _radius(RADIUS_DEFAULT),
    _mousePositions(),
    _mouseButtons(),
    _shapePixmap(),
    _areaPixmap()
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
    
    paint();
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
    
    paint();
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
    
    paint();
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
    auto shouldPaint = true;

    switch (event->type())
    {
        // Prevent recursive paint events
        case QEvent::Paint:
            return false;

        case QEvent::Resize:
        {
            const auto resizeEvent = static_cast<QResizeEvent*>(event);

            _shapePixmap    = QPixmap(resizeEvent->size());
            _areaPixmap     = QPixmap(resizeEvent->size());

            _shapePixmap.fill(Qt::transparent);
            _areaPixmap.fill(Qt::transparent);

            break;
        }
        
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
                            if (mouseEvent->pos() != _mousePositions.last())
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

    if (shouldPaint)
        paint();

    return QObject::eventFilter(target, event);
}

void SelectionTool::paint()
{
    if (!canSelect())
        return;

    _shapePixmap = QPixmap(_shapePixmap.size());
    _shapePixmap.fill(Qt::transparent);

    _areaPixmap = QPixmap(_shapePixmap.size());
    _areaPixmap.fill(Qt::transparent);

    QPainter shapePainter(&_shapePixmap), areaPainter(&_areaPixmap);

    shapePainter.setFont(QFont("Font Awesome 5 Free Solid", 9));

    auto textRectangle = QRectF();

    QVector<QPoint> controlPoints;

    const auto noMousePositions = _mousePositions.size();

    switch (_type)
    {
        case Type::Rectangle:
        {
            if (noMousePositions == 2) {
                const auto topLeft      = QPointF(std::min(_mousePositions.first().x(), _mousePositions.last().x()), std::min(_mousePositions.first().y(), _mousePositions.last().y()));
                const auto bottomRight  = QPointF(std::max(_mousePositions.first().x(), _mousePositions.last().x()), std::max(_mousePositions.first().y(), _mousePositions.last().y()));
                const auto rectangle    = QRectF(topLeft, bottomRight);

                controlPoints << _mousePositions.first();
                controlPoints << _mousePositions.last();
                
                areaPainter.setBrush(AREA_BRUSH);
                areaPainter.setPen(Qt::NoPen);
                areaPainter.drawRect(rectangle);

                shapePainter.setPen(PEN_LINE_FG);
                shapePainter.drawRect(rectangle);

                const auto size         = 8.0f;
                const auto textCenter   = rectangle.topRight() + QPoint(size, -size);

                textRectangle = QRectF(textCenter - QPointF(size, size), textCenter + QPointF(size, size));
            }

            break;
        }

        
        case Type::Brush:
        {
            if (noMousePositions >= 1) {
                const auto brushCenter = _mousePositions.last();

                if (noMousePositions > 1) {
                    areaPainter.setBrush(Qt::NoBrush);
                    areaPainter.setPen(QPen(AREA_BRUSH, 2.0 * _radius, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    areaPainter.drawPolyline(_mousePositions.constData(), _mousePositions.count());
                }
                
                shapePainter.setPen(Qt::NoPen);
                shapePainter.setBrush(AREA_BRUSH);

                shapePainter.drawPoint(brushCenter);

                shapePainter.setPen(_mouseButtons & Qt::LeftButton ? PEN_LINE_FG : PEN_LINE_BG);

                shapePainter.setBrush(Qt::NoBrush);
                shapePainter.drawEllipse(QPointF(brushCenter), _radius, _radius);

                controlPoints << _mousePositions.last();

                const auto textAngle    = 0.75f * M_PI;
                const auto size         = 12.0f;
                const auto textCenter   = brushCenter + (_radius + size) * QPointF(sin(textAngle), cos(textAngle));

                textRectangle = QRectF(textCenter - QPointF(size, size), textCenter + QPointF(size, size));
            }

            break;
        }
        
        case Type::Lasso:
        {
            if (noMousePositions >= 2) {
                shapePainter.setBrush(AREA_BRUSH);
                shapePainter.setPen(Qt::NoPen);
                shapePainter.drawPolygon(_mousePositions.constData(), _mousePositions.count());

                shapePainter.setBrush(Qt::NoBrush);

                shapePainter.setPen(PEN_LINE_FG);
                shapePainter.drawPolyline(_mousePositions.constData(), _mousePositions.count());

                controlPoints << _mousePositions.first();
                controlPoints << _mousePositions.last();

                shapePainter.setBrush(AREA_BRUSH);
                shapePainter.setPen(PEN_LINE_BG);
                shapePainter.drawPolyline(controlPoints.constData(), controlPoints.count());

                const auto size = 8.0f;
                const auto textCenter = _mousePositions.first() - QPoint(size, size);

                textRectangle = QRectF(textCenter - QPointF(size, size), textCenter + QPointF(size, size));
            }

            break;
        }
        
        case Type::Polygon:
        {
            if (noMousePositions >= 2) {
                shapePainter.setBrush(AREA_BRUSH);
                shapePainter.setPen(Qt::NoPen);
                shapePainter.drawPolygon(_mousePositions.constData(), _mousePositions.count());

                shapePainter.setBrush(Qt::NoBrush);

                shapePainter.setPen(PEN_LINE_FG);
                shapePainter.drawPolyline(_mousePositions.constData(), _mousePositions.count());

                QVector<QPoint> connectingPoints;

                connectingPoints << _mousePositions.first();
                connectingPoints << _mousePositions.last();

                shapePainter.setPen(PEN_LINE_BG);
                shapePainter.drawPolyline(connectingPoints.constData(), connectingPoints.count());

                controlPoints << _mousePositions;

                const auto size = 8.0f;
                const auto textCenter = _mousePositions.first() - QPoint(size, size);

                textRectangle = QRectF(textCenter - QPointF(size, size), textCenter + QPointF(size, size));
            }

            break;
        }
       
        default:
            break;
    }
    
    shapePainter.setPen(PEN_CP);
    shapePainter.drawPoints(controlPoints);
    
    switch (_type)
    {
        case Type::Rectangle:
        case Type::Brush:
        case Type::Lasso:
        case Type::Polygon:
        {
            switch (_modifier)
            {
                case Modifier::Replace:
                    break;

                case Modifier::Add:
                {
                    shapePainter.setPen(PEN_LINE_FG);
                    shapePainter.drawText(textRectangle, hdps::Application::getIconFont("FontAwesome").getIconCharacter("plus-circle"), QTextOption(Qt::AlignCenter));
                    break;
                }

                case Modifier::Remove:
                {
                    shapePainter.setPen(PEN_LINE_FG);
                    shapePainter.drawText(textRectangle, hdps::Application::getIconFont("FontAwesome").getIconCharacter("minus-circle"), QTextOption(Qt::AlignCenter));
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

    emit overlayChanged();
}