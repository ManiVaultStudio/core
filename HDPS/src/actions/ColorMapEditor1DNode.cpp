#include "ColorMapEditor1DNode.h"
#include "ColorMapEditor1DEdge.h"
#include "ColorMapEditor1DWidget.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QStyle>
#include <QDebug>
#include <QRadialGradient>

#ifdef _DEBUG
    #define COLOR_MAP_EDITOR_1D_NODE_VERBOSE
#endif

namespace hdps {

namespace gui {

ColorMapEditor1DNode::ColorMapEditor1DNode(ColorMapEditor1DWidget& colorMapEditor1DWidget) :
    QObject(&colorMapEditor1DWidget),
    QGraphicsItem(),
    _colorMapEditor1DWidget(colorMapEditor1DWidget),
    _index(),
    _color(Qt::gray),
    _radius(5.0f),
    _edgeList(),
    _hover(false)
{
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    //setCacheMode(DeviceCoordinateCache);
    setZValue(10);
    setAcceptHoverEvents(true);

    connect(&_colorMapEditor1DWidget, &ColorMapEditor1DWidget::currentNodeChanged, this, [this]() -> void {
        update();
    });

    _colorMapEditor1DWidget.installEventFilter(this);
}

std::uint32_t ColorMapEditor1DNode::getIndex() const
{
    return _index;
}

void ColorMapEditor1DNode::setIndex(const std::uint32_t& index)
{
    _index = index;
}

QColor ColorMapEditor1DNode::getColor() const
{
    return _color;
}

void ColorMapEditor1DNode::setColor(const QColor& color)
{
    if (color == _color)
        return;

    _color = color;

    update();

    emit colorChanged(_color);
}

float ColorMapEditor1DNode::getRadius() const
{
    return _radius;
}

void ColorMapEditor1DNode::setRadius(const float& radius)
{
    if (radius == _radius)
        return;

    _radius = radius;

    update();
}

QPointF ColorMapEditor1DNode::getNormalizedCoordinate() const
{
    const auto graphRectangle = _colorMapEditor1DWidget.getGraphRectangle();

    return QPointF((x() - graphRectangle.x()) / graphRectangle.width(), 1.0f - (y() - graphRectangle.y()) / graphRectangle.height());
}

void ColorMapEditor1DNode::setNormalizedCoordinate(const QPointF& normalizedCoordinate)
{
    if (normalizedCoordinate == getNormalizedCoordinate())
        return;

    const auto graphRectangle = _colorMapEditor1DWidget.getGraphRectangle();

    setPos(graphRectangle.x() + (normalizedCoordinate.x() * graphRectangle.width()), graphRectangle.bottom() - (normalizedCoordinate.y() * graphRectangle.height()));

    emit normalizedCoordinateChanged(normalizedCoordinate);
}

bool ColorMapEditor1DNode::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::EnabledChange:
            update();
            break;

        default:
            break;
    }

    return QObject::eventFilter(target, event);
}

QRectF ColorMapEditor1DNode::boundingRect() const
{
    return QRectF(-_radius, -_radius, 2 * _radius, 2 *_radius).marginsRemoved(QMargins(3, 3, 3, 3));
}

QRectF ColorMapEditor1DNode::getLimits() const
{
    const auto previousNode     = _colorMapEditor1DWidget.getPreviousNode(const_cast<ColorMapEditor1DNode*>(this));
    const auto nextNode         = _colorMapEditor1DWidget.getNextNode(const_cast<ColorMapEditor1DNode*>(this));
    const auto graphRectangle   = _colorMapEditor1DWidget.getGraphRectangle();

    if (this == previousNode)
        return QRectF(QPointF(graphRectangle.left(), graphRectangle.top()), QPointF(graphRectangle.left(), graphRectangle.bottom()));

    if (this == nextNode)
        return QRectF(QPointF(graphRectangle.right(), graphRectangle.top()), QPointF(graphRectangle.right(), graphRectangle.bottom()));

    return QRectF(QPointF(previousNode->scenePos().x(), graphRectangle.top()), QPointF(nextNode->scenePos().x(), graphRectangle.bottom()));
}

QPainterPath ColorMapEditor1DNode::shape() const
{
    QPainterPath path;

    path.addEllipse(boundingRect().center(), _radius, _radius);

    return path;
}

void ColorMapEditor1DNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    QStyleOption styleOption;

    styleOption.init(&_colorMapEditor1DWidget);

    if (_colorMapEditor1DWidget.isEnabled() && _hover) {
        QRadialGradient radialGradient;

        radialGradient.setCenter(boundingRect().center());
        radialGradient.setCenterRadius(0.5 * boundingRect().width());
        radialGradient.setColorAt(0.0, QColor(100, 100, 100, 100));
        radialGradient.setColorAt(0.9, QColor(100, 100, 100, 100));
        radialGradient.setColorAt(1.0, QColor(100, 100, 100, 100));

        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(Qt::red));
        painter->drawEllipse(boundingRect());
    }

    QPen perimeterPen;

    perimeterPen.setWidth(_colorMapEditor1DWidget.getCurrentNode() == this ? 3.0f : 1.5f);
    perimeterPen.setColor(_colorMapEditor1DWidget.isEnabled() ? styleOption.palette.color(QPalette::Normal, QPalette::ButtonText) : styleOption.palette.color(QPalette::Disabled, QPalette::ButtonText));

    painter->setPen(perimeterPen);

    painter->setBrush(_colorMapEditor1DWidget.isEnabled() ? _color : QColor::fromHsl(_color.hue(), 0, _color.lightness()));
    painter->drawPath(shape());
}

QVariant ColorMapEditor1DNode::itemChange(GraphicsItemChange change, const QVariant& value)
{
    switch (change) {
        case ItemPositionHasChanged:
        {
            for (auto edge : qAsConst(_edgeList))
                edge->update();

            break;
        }

        default:
            break;
    };

    return QGraphicsItem::itemChange(change, value);
}

void ColorMapEditor1DNode::pressed(QGraphicsSceneMouseEvent* event) {
    this->mousePressEvent(event);
}

void ColorMapEditor1DNode::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
#ifdef COLOR_MAP_EDITOR_1D_NODE_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    _colorMapEditor1DWidget.selectNode(this);

    QGraphicsItem::mousePressEvent(event);
}

void ColorMapEditor1DNode::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
#ifdef COLOR_MAP_EDITOR_1D_NODE_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    const auto limits = getLimits();

    qDebug() << limits;

    QPoint scenePos;

    scenePos.setX(std::max(limits.left(), std::min(event->scenePos().x(), limits.right())));
    scenePos.setY(std::max(limits.top(), std::min(event->scenePos().y(), limits.bottom())));

    event->setScenePos(scenePos);

    QGraphicsItem::mouseMoveEvent(event);

    emit normalizedCoordinateChanged(getNormalizedCoordinate());

    update();
}

void ColorMapEditor1DNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    update();

    QGraphicsItem::mouseReleaseEvent(event);
}

void ColorMapEditor1DNode::hoverEnterEvent(QGraphicsSceneHoverEvent* graphicsSceneHoverEvent)
{
#ifdef COLOR_MAP_EDITOR_1D_NODE_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    _hover = true;

    update();
}

void ColorMapEditor1DNode::hoverLeaveEvent(QGraphicsSceneHoverEvent* graphicsSceneHoverEvent)
{
#ifdef COLOR_MAP_EDITOR_1D_NODE_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    _hover = false;

    update();
}

void ColorMapEditor1DNode::addEdge(ColorMapEditor1DEdge* edge)
{
    _edgeList << edge;
}

QVector<ColorMapEditor1DEdge*> ColorMapEditor1DNode::edges() const
{
    return _edgeList;
}

}
}
