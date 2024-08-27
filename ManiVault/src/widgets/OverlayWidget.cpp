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
    QWidget(target->parentWidget()),
    _widgetOverlayer(this, this, target, initialOpacity)
{
}

mv::util::WidgetOverlayer& OverlayWidget::getWidgetOverlayer()
{
    return _widgetOverlayer;
}

}
