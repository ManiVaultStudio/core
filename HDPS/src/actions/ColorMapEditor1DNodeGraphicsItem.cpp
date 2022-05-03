#include "ColorMapEditor1DNodeGraphicsItem.h"
#include "ColorMapEditor1DNode.h"
#include "ColorMapEditor1DWidget.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QStyle>
#include <QDebug>
#include <QRadialGradient>

#ifdef _DEBUG
    #define COLOR_MAP_EDITOR_1D_NODE_GRAPHICS_ITEM_VERBOSE
#endif

namespace hdps {

namespace gui {

ColorMapEditor1DNodeGraphicsItem::ColorMapEditor1DNodeGraphicsItem(ColorMapEditor1DWidget& colorMapEditor1DWidget, ColorMapEditor1DNode& node) :
    QObject(&colorMapEditor1DWidget),
    QGraphicsItem(),
    _colorMapEditor1DWidget(colorMapEditor1DWidget),
    _node(node),
    _hover(false)
{
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setZValue(10);
    setAcceptHoverEvents(true);

    connect(&_colorMapEditor1DWidget, &ColorMapEditor1DWidget::currentNodeChanged, this, [this]() -> void {
        update();
    });

    const auto updatePos = [this]() -> void {
        const auto graphRectangle       = _colorMapEditor1DWidget.getGraphRectangle();
        const auto normalizedCoordinate = _node.getNormalizedCoordinate();

        setPos(graphRectangle.x() + (normalizedCoordinate.x() * graphRectangle.width()), graphRectangle.bottom() - (normalizedCoordinate.y() * graphRectangle.height()));

        _colorMapEditor1DWidget.update();
    };

    connect(&_node, &ColorMapEditor1DNode::normalizedCoordinateChanged, this, updatePos);

    connect(&_node, &ColorMapEditor1DNode::colorChanged, this, [this]() -> void {
        update();
    });

    updatePos();
}

hdps::gui::ColorMapEditor1DNode& ColorMapEditor1DNodeGraphicsItem::getNode()
{
    return _node;
}

bool ColorMapEditor1DNodeGraphicsItem::eventFilter(QObject* target, QEvent* event)
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

QRectF ColorMapEditor1DNodeGraphicsItem::boundingRect() const
{
    return QRectF(-_node.getRadius(), -_node.getRadius(), 2 * _node.getRadius(), 2 * _node.getRadius());// .marginsRemoved(QMargins(3, 3, 3, 3));
}

QPainterPath ColorMapEditor1DNodeGraphicsItem::shape() const
{
    QPainterPath path;

    path.addEllipse(boundingRect().center(), _node.getRadius(), _node.getRadius());

    return path;
}

void ColorMapEditor1DNodeGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    QStyleOption styleOption;

    styleOption.init(&_colorMapEditor1DWidget);

    if (_colorMapEditor1DWidget.isEnabled() && _hover) {
        QPen hoverPen;

        hoverPen.setWidth(7.0f);
        hoverPen.setColor(QColor(10, 10, 10, 100));

        painter->setPen(hoverPen);
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(boundingRect());
        painter->drawPath(shape());
    }

    QPen perimeterPen;

    perimeterPen.setWidth(_colorMapEditor1DWidget.getCurrentNode() == &_node ? 3.0f : 2.0f);
    perimeterPen.setColor(_colorMapEditor1DWidget.isEnabled() ? styleOption.palette.color(QPalette::Normal, QPalette::ButtonText) : styleOption.palette.color(QPalette::Disabled, QPalette::ButtonText));

    painter->setPen(perimeterPen);

    painter->setBrush(_colorMapEditor1DWidget.isEnabled() ? _node.getColor() : Qt::lightGray);
    painter->drawPath(shape());
}

void ColorMapEditor1DNodeGraphicsItem::pressed(QGraphicsSceneMouseEvent* event) {
    this->mousePressEvent(event);
}

void ColorMapEditor1DNodeGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
#ifdef COLOR_MAP_EDITOR_1D_NODE_GRAPHICS_ITEM_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    _colorMapEditor1DWidget.selectNode(&_node);

    QGraphicsItem::mousePressEvent(event);
}

void ColorMapEditor1DNodeGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
#ifdef COLOR_MAP_EDITOR_1D_NODE_GRAPHICS_ITEM_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    const auto limits           = _node.getLimits();
    const auto graphRectangle   = _colorMapEditor1DWidget.getGraphRectangle();
    const auto left             = graphRectangle.left() + (limits.left() * graphRectangle.width());
    const auto right            = graphRectangle.left() + (limits.right() * graphRectangle.width());

    QPoint scenePos;

    scenePos.setX(std::max(left, std::min(event->scenePos().x(), right)));
    scenePos.setY(std::max(graphRectangle.top(), std::min(event->scenePos().y(), graphRectangle.bottom())));

    //event->setScenePos(scenePos);

    _node.setNormalizedCoordinate(QPointF((scenePos.x() - graphRectangle.left()) / graphRectangle.width(), ((_colorMapEditor1DWidget.height() - scenePos.y()) - graphRectangle.top()) / graphRectangle.height()));

    update();
}

void ColorMapEditor1DNodeGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    update();

    QGraphicsItem::mouseReleaseEvent(event);
}

void ColorMapEditor1DNodeGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent* graphicsSceneHoverEvent)
{
#ifdef COLOR_MAP_EDITOR_1D_NODE_GRAPHICS_ITEM_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    _hover = true;

    update();
}

void ColorMapEditor1DNodeGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* graphicsSceneHoverEvent)
{
#ifdef COLOR_MAP_EDITOR_1D_NODE_GRAPHICS_ITEM_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    _hover = false;

    update();
}

}
}
