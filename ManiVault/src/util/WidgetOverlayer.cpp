// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WidgetOverlayer.h"

#include <QDebug>
#include <QWidget>
#include <QResizeEvent>

#ifdef _DEBUG
    #define WIDGET_OVERLAYER_VERBOSE
#endif

namespace mv::util {

WidgetOverlayer::WidgetOverlayer(QObject* parent, QWidget* sourceWidget, QWidget* targetWidget, float initialOpacity /*= 1.0f*/) :
    QObject(parent),
    _sourceWidget(sourceWidget)
{
    setObjectName("WidgetOverlayer");

    Q_ASSERT(_sourceWidget);

    if (!sourceWidget)
        return;

    _sourceWidget->installEventFilter(this);
    //_sourceWidget->setMouseTracking(true);

    setTargetWidget(targetWidget);
}

QWidget* WidgetOverlayer::getSourceWidget() const
{
    return _sourceWidget;
}

QWidget* WidgetOverlayer::getTargetWidget() const
{
    return _targetWidget;
}

void WidgetOverlayer::setTargetWidget(QWidget* targetWidget)
{
    Q_ASSERT(targetWidget);

    if (!targetWidget)
        return;

    //if (targetWidget == _targetWidget)
    //    return;

    auto previousTargetWidget = _targetWidget;

    if (_targetWidget)
        _targetWidget->removeEventFilter(this);

    _targetWidget = targetWidget;

    _targetWidget->setMouseTracking(true);
    _targetWidget->installEventFilter(this);

    synchronizeGeometry();

    emit targetWidgetChanged(previousTargetWidget, _targetWidget);
}

void WidgetOverlayer::addMouseEventReceiverWidget(const QWidget* mouseEventReceiverWidget)
{
    Q_ASSERT(mouseEventReceiverWidget);

    if (!mouseEventReceiverWidget)
        return;

    _mouseEventReceiverWidgets << mouseEventReceiverWidget;

    emit mouseEventReceiverWidgetAdded(mouseEventReceiverWidget);
}

void WidgetOverlayer::removeMouseEventReceiverWidget(const QWidget* mouseEventReceiverWidget)
{
    Q_ASSERT(mouseEventReceiverWidget);

    if (!mouseEventReceiverWidget)
        return;

    _mouseEventReceiverWidgets.removeOne(mouseEventReceiverWidget);

    emit mouseEventReceiverWidgetRemoved(mouseEventReceiverWidget);
}

WidgetOverlayer::MouseEventReceiverWidgets WidgetOverlayer::getMouseEventReceiverWidgets()
{
    return _mouseEventReceiverWidgets;
}

bool WidgetOverlayer::shouldReceiveMouseEvents() const
{
    for (const auto mouseEventReceiverWidget : _mouseEventReceiverWidgets)
        if (mouseEventReceiverWidget->underMouse())
            return true;

    return false;
}

bool WidgetOverlayer::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Resize:
        {
            if (dynamic_cast<QWidget*>(target) == _targetWidget)
                synchronizeGeometry();

            break;
        }

        case QEvent::Show:
        {
            if (dynamic_cast<QWidget*>(target) == _sourceWidget)
                _sourceWidget->raise();

            break;
        }

        default:
            break;
    }

    return QObject::eventFilter(target, event);
}

void WidgetOverlayer::synchronizeGeometry() const
{
    _sourceWidget->resize(_targetWidget->size());
}

}
