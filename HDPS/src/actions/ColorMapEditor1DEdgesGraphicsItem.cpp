// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ColorMapEditor1DEdgesGraphicsItem.h"
#include "ColorMapEditor1DNode.h"
#include "ColorMapEditor1DWidget.h"

#include <QGraphicsRectItem>
#include <QPainter>
#include <QEvent>
#include <QStyleOption>
#include <QDebug>

#ifdef _DEBUG
    #define COLOR_MAP_EDITOR_1D_EDGES_VERBOSE
#endif

namespace hdps {

namespace gui {

ColorMapEditor1DEdgesGraphicsItem::ColorMapEditor1DEdgesGraphicsItem(ColorMapEditor1DWidget& colorMapEditor1DWidget) :
    QObject(&colorMapEditor1DWidget),
    QGraphicsItem(),
    _colorMapEditor1DWidget(colorMapEditor1DWidget)
{
    setAcceptedMouseButtons(Qt::NoButton);
    setZValue(9);

    _colorMapEditor1DWidget.installEventFilter(this);
}

bool ColorMapEditor1DEdgesGraphicsItem::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::EnabledChange:
        {
            if (target == &_colorMapEditor1DWidget)
                update();

            break;
        }

        default:
            break;
    }

    return QObject::eventFilter(target, event);
}

QRectF ColorMapEditor1DEdgesGraphicsItem::boundingRect() const
{
    return QRectF();
}

void ColorMapEditor1DEdgesGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
#ifdef COLOR_MAP_EDITOR_1D_EDGES_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    QVector<QPoint> points;

    const auto graphRectangle = _colorMapEditor1DWidget.getGraphRectangle();

    for (auto node : _colorMapEditor1DWidget.getNodes())
        points << QPoint(graphRectangle.left() + (node->getNormalizedCoordinate().x() * graphRectangle.width()), graphRectangle.bottom() - (node->getNormalizedCoordinate().y() * graphRectangle.height()));

    QStyleOption styleOption;

    styleOption.initFrom(&_colorMapEditor1DWidget);

    QPen pen;

    pen.setWidth(2);
    pen.setColor(_colorMapEditor1DWidget.isEnabled() ? styleOption.palette.color(QPalette::Normal, QPalette::ButtonText) : styleOption.palette.color(QPalette::Disabled, QPalette::ButtonText));

    painter->setPen(pen);
    painter->drawPolyline(points.data(), points.count());
}

}
}
