// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "OverlayWidget.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>

#ifdef _DEBUG
    #define OVERLAY_WIDGET_VERBOSE
#endif

namespace mv::gui
{

OverlayWidget::OverlayWidget(QWidget* target, float initialOpacity /*= 1.0f*/) :
    QWidget(target),
    _widgetOverlayer(this, this, target, initialOpacity)
{
    setObjectName("OverlayWidget");

    setMouseTracking(true);
    //setAttribute(Qt::WA_TransparentForMouseEvents, false);
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

    //setMask(reg);
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

    QRegion circularRegion(QRect(0, 0, 1000, 20), QRegion::Rectangle);

    QRegion reg(geometry());

    reg -= QRegion(geometry());
    reg += childrenRegion();

    setMask(reg);

    qDebug() << childrenRegion();
}

}
