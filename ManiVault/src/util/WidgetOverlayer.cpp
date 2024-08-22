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
    _sourceWidget(sourceWidget),
    _targetWidget()
{
    setObjectName("WidgetOverlayer");

    Q_ASSERT(_sourceWidget);

    if (!sourceWidget)
        return;

    _sourceWidget->setMouseTracking(true);
    _sourceWidget->installEventFilter(this);

    setTargetWidget(targetWidget);
}

QWidget* WidgetOverlayer::getSourceWidget()
{
    return _sourceWidget;
}

QWidget* WidgetOverlayer::getTargetWidget()
{
    return _targetWidget;
}

void WidgetOverlayer::setTargetWidget(QWidget* targetWidget)
{
    Q_ASSERT(targetWidget);

    if (!targetWidget)
        return;

    if (_targetWidget)
        _targetWidget->removeEventFilter(this);

    _targetWidget = targetWidget;

    _targetWidget->installEventFilter(this);

    _sourceWidget->resize(_targetWidget->size());
}

bool WidgetOverlayer::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Resize:
        {
            if (dynamic_cast<QWidget*>(target) != _targetWidget)
                break;

            _sourceWidget->setGeometry(_targetWidget->geometry());

            break;
        }

        case QEvent::Show:
        {
            if (dynamic_cast<QWidget*>(target) != _sourceWidget)
                break;

            _sourceWidget->raise();

            break;
        }

        //case QEvent::Enter:
        //case QEvent::Leave:
        //case QEvent::MouseMove:
        //case QEvent::MouseButtonPress:
        //case QEvent::MouseButtonRelease:
        //{
        //    if (dynamic_cast<QWidget*>(target) == _sourceWidget)
        //        return true;

        //    break;
        //}

        default:
            break;
    }

    return QObject::eventFilter(target, event);
}

}
