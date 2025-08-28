// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "OverlayWidget.h"

#include <QDebug>

#include <QPainterPath>
#include <QRegion>

#ifdef _DEBUG
    #define OVERLAY_WIDGET_VERBOSE
#endif

using namespace mv::util;

namespace mv::gui
{

OverlayWidget::OverlayWidget(QWidget* target, float initialOpacity /*= 1.0f*/) :
    QWidget(target),
    _widgetOverlayer(this, this, target, initialOpacity)
{
    setObjectName("OverlayWidget");
    setMouseTracking(true);

    connect(&_widgetOverlayer, &WidgetOverlayer::mouseEventReceiverWidgetAdded, this, &OverlayWidget::updateMask);
    connect(&_widgetOverlayer, &WidgetOverlayer::mouseEventReceiverWidgetRemoved, this, &OverlayWidget::updateMask);
}

mv::util::WidgetOverlayer& OverlayWidget::getWidgetOverlayer()
{
    return _widgetOverlayer;
}

void OverlayWidget::addMouseEventReceiverWidget(QWidget* mouseEventReceiverWidget)
{
    Q_ASSERT(mouseEventReceiverWidget);

    if (!mouseEventReceiverWidget)
        return;

    _widgetOverlayer.addMouseEventReceiverWidget(mouseEventReceiverWidget);
}

void OverlayWidget::removeMouseEventReceiverWidget(QWidget* mouseEventReceiverWidget)
{
    Q_ASSERT(mouseEventReceiverWidget);

    if (!mouseEventReceiverWidget)
        return;

    _widgetOverlayer.removeMouseEventReceiverWidget(mouseEventReceiverWidget);
}

void OverlayWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    updateMask();
}

void OverlayWidget::updateMask()
{
    updateGeometry();

    QRegion maskRegion(geometry());

    maskRegion -= QRegion(QRect(geometry()));

    for (auto mouseEventReceiverWidget : _widgetOverlayer.getMouseEventReceiverWidgets())
		maskRegion += QRect(mapFromGlobal(mapToGlobal(mouseEventReceiverWidget->pos())), mouseEventReceiverWidget->size());

    setMask(maskRegion);
}

}
