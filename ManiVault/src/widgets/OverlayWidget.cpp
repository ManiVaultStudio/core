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
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
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

    mouseEventReceiverWidget->installEventFilter(this);
}

void OverlayWidget::removeMouseEventReceiverWidget(QWidget* mouseEventReceiverWidget)
{
    Q_ASSERT(mouseEventReceiverWidget);

    if (!mouseEventReceiverWidget)
        return;

    _widgetOverlayer.removeMouseEventReceiverWidget(mouseEventReceiverWidget);

    mouseEventReceiverWidget->removeEventFilter(this);
}

//bool OverlayWidget::event(QEvent* event)
//{
//    switch (event->type())
//    {
//        case QEvent::Enter:
//        case QEvent::Leave:
//        case QEvent::MouseButtonPress:
//        case QEvent::MouseButtonRelease:
//        case QEvent::MouseMove:
//        {
//            //if (!_widgetOverlayer.shouldReceiveMouseEvents())
//                //propagateMouseEvent(dynamic_cast<QMouseEvent*>(event));
//            event->ignore();
//            break;
//        }
//    }
//
//    return QWidget::event(event);
//}

//void OverlayWidget::enterEvent(QEnterEvent* event)
//{
//    if (!_widgetOverlayer.shouldReceiveMouseEvents())
//        QCoreApplication::sendEvent(parent(), event);
//}
//
void OverlayWidget::mousePressEvent(QMouseEvent* event)
{
    //event->ignore();
    if (!_widgetOverlayer.shouldReceiveMouseEvents())
        propagateMouseEvent(event);
}

void OverlayWidget::mouseReleaseEvent(QMouseEvent* event)
{
    //event->ignore();
    if (!_widgetOverlayer.shouldReceiveMouseEvents())
        propagateMouseEvent(event);
}

void OverlayWidget::mouseMoveEvent(QMouseEvent* event)
{
    //event->ignore();
    if (!_widgetOverlayer.shouldReceiveMouseEvents())
        propagateMouseEvent(event);

    //qDebug() << __FUNCTION__;
    //QCoreApplication::sendEvent(parent(), event);
}

void OverlayWidget::propagateMouseEvent(QMouseEvent* event)
{
    //qDebug() << __FUNCTION__ << parent()->metaObject()->className() << parent()->objectName() << _widgetOverlayer.getTargetWidget()->objectName();

    const auto positionInParentWidget = mapToParent(event->pos());

    

    //qDebug() << this << targetWidget << parentWidget()->children();

    for (auto child : parentWidget()->children())
        if (auto childWidget = dynamic_cast<QWidget*>(child)) {
            

            if (childWidget !=this && childWidget->geometry().contains(positionInParentWidget))
            {
                auto targetWidget   = childWidget->childAt(positionInParentWidget);
                auto mouseEvent     = new QMouseEvent(event->type(), positionInParentWidget, event->globalPos(), event->button(), event->buttons(), event->modifiers());

                //QCoreApplication::sendEvent(parentWidget(), mouseEvent);
                //QCoreApplication::sendEvent(childWidget, mouseEvent);
                QCoreApplication::postEvent(targetWidget, mouseEvent);

                event->ignore();

                qDebug() << "    " << childWidget << targetWidget << childWidget->geometry().contains(positionInParentWidget) << childWidget->geometry();
            }
        }

    //if (targetWidget == this)
    //    return;

    //targetWidget->setStyleSheet("background-color: red");
    //return;
    
}

//bool OverlayWidget::eventFilter(QObject* watched, QEvent* event)
//{
//    switch (event->type())
//    {
//        case QEvent::MouseButtonPress:
//        case QEvent::MouseButtonRelease:
//        case QEvent::MouseMove:
//            return false;
//    }
//
//    return QWidget::eventFilter(watched, event);
//}

}
