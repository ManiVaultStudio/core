// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "OverlayWidget.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>

#ifdef _DEBUG
    #define OVERLAY_WIDGET_VERBOSE
#endif

namespace hdps::gui
{

OverlayWidget::OverlayWidget(QWidget* parent, float initialOpacity /*= 1.0f*/) :
    QWidget(parent),
    _widgetOverlayer(this, this, parent, initialOpacity)
{
}

hdps::util::WidgetOverlayer& OverlayWidget::getWidgetOverlayer()
{
    return _widgetOverlayer;
}

}
