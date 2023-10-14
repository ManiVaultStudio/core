// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WidgetOverlayer.h"

#include <QDebug>
#include <QWidget>
#include <QVBoxLayout>
#include <QResizeEvent>

#ifdef _DEBUG
    #define WIDGET_OVERLAYER_VERBOSE
#endif

namespace mv::util {

WidgetOverlayer::WidgetOverlayer(QObject* parent, QWidget* sourceWidget, QWidget* targetWidget, float initialOpacity /*= 1.0f*/) :
    QObject(parent),
    _widgetFader(this, sourceWidget, initialOpacity, 0.0f, 1.0f, 120, 60),
    _sourceWidget(sourceWidget),
    _targetWidget(targetWidget)
{
    Q_ASSERT(_sourceWidget != nullptr);
    Q_ASSERT(_targetWidget != nullptr);
    
    setObjectName("WidgetOverlayer");

    _sourceWidget->setAttribute(Qt::WA_TransparentForMouseEvents);

    _sourceWidget->installEventFilter(this);
    _targetWidget->installEventFilter(this);
}

bool WidgetOverlayer::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Resize:
        {
            if (dynamic_cast<QWidget*>(target) != _targetWidget)
                break;

            _sourceWidget->setFixedSize(static_cast<QResizeEvent*>(event)->size());

            break;
        }

        case QEvent::Show:
        {
            if (dynamic_cast<QWidget*>(target) != _sourceWidget)
                break;

            _sourceWidget->raise();

            break;
        }

        default:
            break;
    }

    return QObject::eventFilter(target, event);
}

mv::util::WidgetFader& WidgetOverlayer::getWidgetFader()
{
    return _widgetFader;
}

}
