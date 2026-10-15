// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ColorMapEditor1DHistogramGraphicsItem.h"
#include "ColorMapEditor1DWidget.h"
#include "ColorMapEditor1DAction.h"

#include <QGraphicsRectItem>
#include <QPainter>
#include <QEvent>
#include <QStyleOption>
#include <QDebug>

#ifdef _DEBUG
    #define COLOR_MAP_EDITOR_1D_HISTOGRAM_GRAPHICS_ITEM_VERBOSE
#endif

namespace mv {

namespace gui {

ColorMapEditor1DHistogramGraphicsItem::ColorMapEditor1DHistogramGraphicsItem(ColorMapEditor1DWidget& colorMapEditor1DWidget) :
    QObject(&colorMapEditor1DWidget),
    QGraphicsItem(),
    _colorMapEditor1DWidget(colorMapEditor1DWidget)
{
    setAcceptedMouseButtons(Qt::NoButton);
    setZValue(8);

    _colorMapEditor1DWidget.installEventFilter(this);

    connect(&colorMapEditor1DWidget.getColorMapEditor1DAction(), &ColorMapEditor1DAction::histogramChanged, this, [this]() -> void {
        update();
    });
}

bool ColorMapEditor1DHistogramGraphicsItem::eventFilter(QObject* target, QEvent* event)
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

QRectF ColorMapEditor1DHistogramGraphicsItem::boundingRect() const
{
    return QRectF();
}

void ColorMapEditor1DHistogramGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
#ifdef COLOR_MAP_EDITOR_1D_HISTOGRAM_GRAPHICS_ITEM_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    QVector<QPointF> points;

    const auto graphRectangle   = _colorMapEditor1DWidget.getGraphRectangle();
    const auto histogram        = _colorMapEditor1DWidget.getColorMapEditor1DAction().getHistogram();

    if (histogram.isEmpty())
        return;

    const auto binMax = 1.1f * *std::max_element(histogram.begin(), histogram.end());

    std::uint32_t binIndex = 0;

    for (const auto& bin : histogram) {
        const auto binNormalized = QPointF(static_cast<float>(binIndex) / (histogram.count() - 1), static_cast<float>(bin) / binMax);

        points << graphRectangle.bottomLeft() + QPointF(binNormalized.x() * graphRectangle.width(), -binNormalized.y() * graphRectangle.height());

        binIndex++;
    }

    QStyleOption styleOption;

    styleOption.initFrom(&_colorMapEditor1DWidget);

    QPen pen;

    pen.setWidthF(1.5f);
    pen.setColor(_colorMapEditor1DWidget.isEnabled() ? QColor(150, 150, 150, 100) : QColor(150, 150, 150, 40));

    painter->setPen(pen);
    painter->drawPolyline(points.data(), points.count());

    points.insert(0, graphRectangle.bottomLeft());
    points << graphRectangle.bottomRight();

    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(150, 150, 150, _colorMapEditor1DWidget.isEnabled() ? 50 : 20));
    painter->drawPolygon(points.data(), points.count());
}

}
}
