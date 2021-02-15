#include "PixelSelectionTool.h"
#include "ScatterplotPlugin.h"
#include "Application.h"

#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QtMath>

const QMap<QString, PixelSelectionTool::Type> PixelSelectionTool::types = {
    { "Rectangle", PixelSelectionTool::Type::Rectangle },
    { "Brush", PixelSelectionTool::Type::Brush },
    { "Lasso", PixelSelectionTool::Type::Lasso },
    { "Polygon", PixelSelectionTool::Type::Polygon }
};

const QMap<QString, PixelSelectionTool::Modifier> PixelSelectionTool::modifiers = {
    { "Replace", PixelSelectionTool::Modifier::Replace },
    { "Add", PixelSelectionTool::Modifier::Add },
    { "Remove", PixelSelectionTool::Modifier::Remove }
};

const QColor PixelSelectionTool::COLOR_MAIN      = Qt::black;
const QColor PixelSelectionTool::COLOR_FILL      = QColor(COLOR_MAIN.red(), COLOR_MAIN.green(), COLOR_MAIN.blue(), 50);
const QBrush PixelSelectionTool::AREA_BRUSH      = QBrush(COLOR_FILL);
const QPen PixelSelectionTool::PEN_LINE_FG       = QPen(COLOR_MAIN, 1.7f, Qt::SolidLine);
const QPen PixelSelectionTool::PEN_LINE_BG       = QPen(QColor(COLOR_MAIN.red(), COLOR_MAIN.green(), COLOR_MAIN.blue(), 140), 1.7f, Qt::DotLine);
const QPen PixelSelectionTool::PEN_CP            = QPen(COLOR_MAIN, 7.0f, Qt::SolidLine, Qt::RoundCap);

PixelSelectionTool::PixelSelectionTool(QObject* parent, const bool& enabled /*= true*/) :
    QObject(parent),
    _enabled(enabled),
    _type(Type::Rectangle),
    _modifier(Modifier::Replace),
    _active(false),
    _notifyDuringSelection(true),
    _brushRadius(BRUSH_RADIUS_DEFAULT),
    _mousePosition(),
    _mousePositions(),
    _mouseButtons(),
    _shapePixmap(),
    _areaPixmap(),
    _preventContextMenu(false)
{
}

bool PixelSelectionTool::isEnabled() const
{
    return _enabled;
}

void PixelSelectionTool::setEnabled(const bool& enabled)
{
    _enabled = enabled;
}

PixelSelectionTool::Type PixelSelectionTool::getType() const
{
    return _type;
}

void PixelSelectionTool::setType(const Type& type)
{
    if (type == _type)
        return;

    _type = type;

    emit typeChanged(_type);
    
    abort();
    paint();
}

PixelSelectionTool::Modifier PixelSelectionTool::getModifier() const
{
    return _modifier;
}

void PixelSelectionTool::setModifier(const Modifier& modifier)
{
    if (modifier == _modifier)
        return;

    _modifier = modifier;

    emit modifierChanged(_modifier);
    
    paint();
}

bool PixelSelectionTool::isNotifyDuringSelection() const
{
    return _notifyDuringSelection;
}

void PixelSelectionTool::setNotifyDuringSelection(const bool& notifyDuringSelection)
{
    if (notifyDuringSelection == _notifyDuringSelection)
        return;

    _notifyDuringSelection = notifyDuringSelection;

    emit notifyDuringSelectionChanged(_notifyDuringSelection);
}

float PixelSelectionTool::getBrushRadius() const
{
    return _brushRadius;
}

void PixelSelectionTool::setBrushRadius(const float& brushRadius)
{
    if (brushRadius == _brushRadius)
        return;

    _brushRadius = std::clamp(brushRadius, BRUSH_RADIUS_MIN, BRUSH_RADIUS_MAX);

    emit brushRadiusChanged(_brushRadius);
    
    paint();
}

void PixelSelectionTool::setChanged()
{
    emit typeChanged(_type);
    emit modifierChanged(_modifier);
    emit notifyDuringSelectionChanged(_notifyDuringSelection);
    emit brushRadiusChanged(_brushRadius);
}

void PixelSelectionTool::abort()
{
    endSelection();
}

bool PixelSelectionTool::eventFilter(QObject* target, QEvent* event)
{
    auto shouldPaint = false;

    switch (event->type())
    {
        // Prevent recursive paint events
        case QEvent::Paint:
            return false;

        case QEvent::ContextMenu:
        {
            /*
            if (_preventContextMenu) {
                _preventContextMenu = false;
                return true;
            }
            */

            //return true;
            qDebug() << "PixelSelectionTool::eventFilter";
            
            break;
        }

        case QEvent::Resize:
        {
            const auto resizeEvent = static_cast<QResizeEvent*>(event);

            _shapePixmap    = QPixmap(resizeEvent->size());
            _areaPixmap     = QPixmap(resizeEvent->size());

            _shapePixmap.fill(Qt::transparent);
            _areaPixmap.fill(Qt::transparent);

            shouldPaint = true;

            break;
        }
        
        case QEvent::MouseButtonPress:
        {
            auto mouseEvent = static_cast<QMouseEvent*>(event);

            _mouseButtons = mouseEvent->buttons();

            switch (_type)
            {
                case Type::Rectangle:
                case Type::Brush:
                case Type::Lasso:
                {
                    switch (mouseEvent->button())
                    {
                        case Qt::LeftButton:
                            startSelection();
                            _mousePositions.clear();
                            _mousePositions << mouseEvent->pos();
                            break;

                        case Qt::RightButton:
                            break;

                        default:
                            break;
                    }
                    
                    break;
                }

                case Type::Polygon:
                {
                    if (_mousePositions.isEmpty() && mouseEvent->button() == Qt::LeftButton)
                        startSelection();

                    switch (mouseEvent->button())
                    {
                        case Qt::LeftButton:
                            _mousePositions << mouseEvent->pos() << mouseEvent->pos();
                            break;

                        case Qt::RightButton:
                            _mousePositions << mouseEvent->pos();
                            return true;
                            break;

                        default:
                            break;
                    }

                    break;
                }

                default:
                    break;
            }

            shouldPaint = true;

            break;
        }

        case QEvent::MouseButtonRelease:
        {
            auto mouseEvent = static_cast<QMouseEvent*>(event);

            _mouseButtons = mouseEvent->buttons();

            switch (_type)
            {
                case Type::Rectangle:
                case Type::Brush:
                case Type::Lasso:
                {
                    switch (mouseEvent->button())
                    {
                        case Qt::LeftButton:
                            endSelection();
                            break;

                        case Qt::RightButton:
                            break;

                        default:
                            break;
                    }

                    break;
                }

                case Type::Polygon:
                {
                    switch (mouseEvent->button())
                    {
                        case Qt::LeftButton:
                            break;

                        case Qt::RightButton:
                            endSelection();
                            break;
 
                        default:
                            break;
                    }

                    break;
                }

                default:
                    break;
            }

            shouldPaint = true;

            break;
        }

        case QEvent::MouseMove:
        {
            auto mouseEvent = static_cast<QMouseEvent*>(event);

            _mousePosition = mouseEvent->pos();

            switch (_type)
            {
                case Type::Rectangle:
                {
                    if (mouseEvent->buttons() & Qt::LeftButton) {
                        if (_mousePositions.size() == 1)
                            _mousePositions << _mousePosition;
                        else
                            _mousePositions.last() = _mousePosition;
                    }

                    if (isActive())
                        shouldPaint = true;

                    break;
                }

                case Type::Brush:
                {
                    if (mouseEvent->buttons() & Qt::LeftButton)
                        _mousePositions << _mousePosition;
                    
                    shouldPaint = true;

                    break;
                }

                case Type::Lasso:
                {
                    if (mouseEvent->buttons() & Qt::LeftButton && !_mousePositions.isEmpty())
                        _mousePositions << mouseEvent->pos();

                    if (isActive())
                        shouldPaint = true;

                    break;
                }

                case Type::Polygon:
                {
                    if (!_mousePositions.isEmpty())
                        _mousePositions.last() = mouseEvent->pos();

                    if (isActive())
                        shouldPaint = true;

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
                        setBrushRadius(_brushRadius - BRUSH_RADIUS_DELTA);
                    else
                        setBrushRadius(_brushRadius + BRUSH_RADIUS_DELTA);

                    shouldPaint = true;

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

void PixelSelectionTool::paint()
{
    if (!_enabled)
        return;

    _shapePixmap = QPixmap(_shapePixmap.size());
    _shapePixmap.fill(Qt::transparent);

    _areaPixmap = QPixmap(_shapePixmap.size());
    _areaPixmap.fill(Qt::transparent);

    QPainter shapePainter(&_shapePixmap), areaPainter(&_areaPixmap);

    shapePainter.setRenderHint(QPainter::Antialiasing);
    areaPainter.setRenderHint(QPainter::Antialiasing);

    shapePainter.setFont(QFont("Font Awesome 5 Free Solid", 9));

    auto textRectangle = QRectF();

    QVector<QPoint> controlPoints;

    const auto noMousePositions = _mousePositions.size();

    switch (_type)
    {
        case Type::Rectangle:
        {
            if (noMousePositions != 2)
                break;

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

            break;
        }
        
        case Type::Brush:
        {
            const auto brushCenter = _mousePosition;

            if (noMousePositions >= 1) {
                if (noMousePositions == 1) {
                    areaPainter.setBrush(AREA_BRUSH);
                    areaPainter.drawEllipse(QPointF(brushCenter), _brushRadius, _brushRadius);
                }
                else {
                    areaPainter.setBrush(Qt::NoBrush);
                    areaPainter.setPen(QPen(AREA_BRUSH, 2.0 * _brushRadius, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    areaPainter.drawPolyline(_mousePositions.constData(), _mousePositions.count());
                }
            }

            shapePainter.setPen(Qt::NoPen);
            shapePainter.setBrush(AREA_BRUSH);

            shapePainter.drawPoint(brushCenter);

            shapePainter.setPen(_mouseButtons & Qt::LeftButton ? PEN_LINE_FG : PEN_LINE_BG);
            shapePainter.setBrush(Qt::NoBrush);
            shapePainter.drawEllipse(QPointF(brushCenter), _brushRadius, _brushRadius);

            controlPoints << brushCenter;

            const auto textAngle    = 0.75f * M_PI;
            const auto size         = 12.0f;
            const auto textCenter   = brushCenter + (_brushRadius + size) * QPointF(sin(textAngle), cos(textAngle));

            textRectangle = QRectF(textCenter - QPointF(size, size), textCenter + QPointF(size, size));

            break;
        }
        
        case Type::Lasso:
        {
            if (noMousePositions < 2)
                break;

            areaPainter.setBrush(AREA_BRUSH);
            areaPainter.setPen(Qt::NoPen);
            areaPainter.drawPolygon(_mousePositions.constData(), _mousePositions.count());

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

            break;
        }
        
        case Type::Polygon:
        {
            if (noMousePositions < 2)
                break;

            areaPainter.setBrush(AREA_BRUSH);
            areaPainter.setPen(Qt::NoPen);
            areaPainter.drawPolygon(_mousePositions.constData(), _mousePositions.count());

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
                    shapePainter.setPen(PEN_LINE_FG);
                    shapePainter.drawText(textRectangle, hdps::Application::getIconFont("FontAwesome").getIconCharacter("plus-circle"), QTextOption(Qt::AlignCenter));
                    break;

                case Modifier::Remove:
                    shapePainter.setPen(PEN_LINE_FG);
                    shapePainter.drawText(textRectangle, hdps::Application::getIconFont("FontAwesome").getIconCharacter("minus-circle"), QTextOption(Qt::AlignCenter));
                    break;

                default:
                    break;
            }

            break;
        }

        default:
            break;
    }

    emit shapeChanged();
    emit areaChanged();
}

void PixelSelectionTool::startSelection()
{
    _active = true;

    emit started();
}

void PixelSelectionTool::endSelection()
{
    emit ended();

    _mousePositions.clear();
    _active = false;
}