#include "ColorMapEditor1DNode.h"
#include "ColorMapEditor1DAction.h"

#include <QDebug>

#ifdef _DEBUG
    #define COLOR_MAP_EDITOR_1D_NODE_VERBOSE
#endif

namespace hdps {

namespace gui {

ColorMapEditor1DNode::ColorMapEditor1DNode(ColorMapEditor1DAction& colorMapEditor1DAction, const QPointF& normalizedCoordinate) :
    QObject(&colorMapEditor1DAction),
    _colorMapEditor1DAction(colorMapEditor1DAction),
    _index(),
    _normalizedCoordinate(normalizedCoordinate),
    _color(Qt::gray),
    _radius(5.0f)
{
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
}

QPointF ColorMapEditor1DNode::getNormalizedCoordinate() const
{
    /*
    const auto graphRectangle = _colorMapEditor1DWidget.getGraphRectangle();

    return QPointF((x() - graphRectangle.x()) / graphRectangle.width(), 1.0f - (y() - graphRectangle.y()) / graphRectangle.height());
    */

    return _normalizedCoordinate;
}

void ColorMapEditor1DNode::setNormalizedCoordinate(const QPointF& normalizedCoordinate)
{
    if (normalizedCoordinate == _normalizedCoordinate)
        return;

    _normalizedCoordinate = normalizedCoordinate;

    emit normalizedCoordinateChanged(normalizedCoordinate);

    /*
    if (normalizedCoordinate == getNormalizedCoordinate())
        return;

    const auto graphRectangle = _colorMapEditor1DWidget.getGraphRectangle();

    setPos(graphRectangle.x() + (normalizedCoordinate.x() * graphRectangle.width()), graphRectangle.bottom() - (normalizedCoordinate.y() * graphRectangle.height()));

    emit normalizedCoordinateChanged(normalizedCoordinate);
    */
}

QRectF ColorMapEditor1DNode::getLimits() const
{
    const auto previousNode     = _colorMapEditor1DAction.getPreviousNode(const_cast<ColorMapEditor1DNode*>(this));
    const auto nextNode         = _colorMapEditor1DAction.getNextNode(const_cast<ColorMapEditor1DNode*>(this));

    if (this == previousNode)
        return QRectF(QPointF(0.0f, 0.0f), QPointF(0.0f, 1.0f));

    if (this == nextNode)
        return QRectF(QPointF(1.0f, 0.0f), QPointF(1.0f, 1.0f));

    return QRectF(QPointF(previousNode->getNormalizedCoordinate().x(), 0.0f), QPointF(nextNode->getNormalizedCoordinate().x(), 1.0f));
}

}
}
