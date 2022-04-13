#include "ColorMapEditor1DNode.h"
#include "ColorMapEditor1DEdge.h"
#include "ColorMapEditor1DWidget.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QStyle>
#include <QDebug>

#define COLOR_MAP_EDITOR_1D_NODE_VERBOSE

namespace hdps {

namespace gui {

ColorMapEditor1DNode::ColorMapEditor1DNode(ColorMapEditor1DWidget& colorMapEditor1DWidget) :
    QObject(&colorMapEditor1DWidget),
    QGraphicsItem(),
    _colorMapEditor1DWidget(colorMapEditor1DWidget),
    _index(),
    _color(Qt::gray),
    _radius(4.0f),
    _edgeList()
{
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    //setZValue(-1);

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
    return QRectF(-_radius, -_radius, 2 * _radius, 2 *_radius).marginsAdded(QMargins(3, 3, 3, 3));
}

QPainterPath ColorMapEditor1DNode::shape() const
{
    QPainterPath path;

    path.addEllipse(0, 0, 2 * _radius, 2 * _radius);

    return path;
}

void ColorMapEditor1DNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    QStyleOption styleOption;

    styleOption.init(&_colorMapEditor1DWidget);

    QPen perimeterPen;

    perimeterPen.setWidth(_colorMapEditor1DWidget.getCurrentNode() == this ? 3 : 2);
    perimeterPen.setColor(_colorMapEditor1DWidget.isEnabled() ? styleOption.palette.color(QPalette::Normal, QPalette::ButtonText) : styleOption.palette.color(QPalette::Disabled, QPalette::ButtonText));

    painter->setPen(perimeterPen);

    painter->setBrush(_color);
    painter->drawEllipse(boundingRect());
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

    const auto valueMin = _colorMapEditor1DWidget.getPreviousNode(this)->x();
    const auto valueMax = _colorMapEditor1DWidget.getNextNode(this)->x();

    event->setPos(QPointF(std::max(valueMin, std::min(event->pos().x(), valueMax)), event->pos().y()));

    QGraphicsItem::mouseMoveEvent(event);

    emit normalizedCoordinateChanged(getNormalizedCoordinate());
}

void ColorMapEditor1DNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    update();

    QGraphicsItem::mouseReleaseEvent(event);

    _colorMapEditor1DWidget.redrawEdges();
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
