// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ColorMapEditor1DNode.h"
#include "ColorMapEditor1DAction.h"

#include <QDebug>

#ifdef _DEBUG
    #define COLOR_MAP_EDITOR_1D_NODE_VERBOSE
#endif

namespace hdps::gui {

ColorMapEditor1DNode::ColorMapEditor1DNode(ColorMapEditor1DAction& colorMapEditor1DAction, const QPointF& normalizedCoordinate, const QColor& color /*= Qt::gray*/) :
    QObject(&colorMapEditor1DAction),
    _colorMapEditor1DAction(colorMapEditor1DAction),
    _index(),
    _normalizedCoordinate(normalizedCoordinate),
    _color(color),
    _radius(4.0f)
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
    return _normalizedCoordinate;
}

void ColorMapEditor1DNode::setNormalizedCoordinate(const QPointF& normalizedCoordinate)
{
    if (normalizedCoordinate == _normalizedCoordinate)
        return;

    _normalizedCoordinate = normalizedCoordinate;

    emit normalizedCoordinateChanged(normalizedCoordinate);
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

void ColorMapEditor1DNode::fromVariantMap(const QVariantMap& variantMap)
{
    Serializable::fromVariantMap(variantMap);

    _normalizedCoordinate.setX(variantMap["NormX"].toReal());
    _normalizedCoordinate.setY(variantMap["NormY"].toReal());
    
    _color.setRed(variantMap["ColorR"].toInt());
    _color.setGreen(variantMap["ColorG"].toInt());
    _color.setBlue(variantMap["ColorB"].toInt());

    _radius = variantMap["Radius"].toFloat();
}

QVariantMap ColorMapEditor1DNode::toVariantMap() const
{
    QVariantMap variantMap = Serializable::toVariantMap();

    variantMap.insert({
        { "NormX", QVariant::fromValue(_normalizedCoordinate.x()) },
        { "NormY", QVariant::fromValue(_normalizedCoordinate.y()) },
        { "ColorR", QVariant::fromValue(_color.red()) },
        { "ColorG", QVariant::fromValue(_color.green()) },
        { "ColorB", QVariant::fromValue(_color.blue()) }
    });

    return variantMap;
}

}
