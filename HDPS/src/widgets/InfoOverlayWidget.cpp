// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "InfoOverlayWidget.h"

#include <QDebug>

#ifdef _DEBUG
    #define INFO_OVERLAY_WIDGET_VERBOSE
#endif

namespace mv::gui
{

InfoOverlayWidget::InfoOverlayWidget(QWidget* parent) :
    InfoWidget(parent),
    _widgetOverlayer(this, this, parent),
    _widgetFader(this, this, 1.f, 0.f, 1.f, 120, 60)
{
    setColors(Qt::black, Qt::lightGray);
}

InfoOverlayWidget::InfoOverlayWidget(QWidget* parent, const QIcon& icon, const QString& title, const QString& description /*= ""*/, const QColor foregroundColor /*= Qt::black*/, const QColor backgroundColor /*= Qt::lightGray*/) :
    InfoOverlayWidget(parent)
{
    set(icon, title, description);
    setColors(foregroundColor, backgroundColor);
}

mv::util::WidgetFader& InfoOverlayWidget::getWidgetFader()
{
    return _widgetFader;
}

}
