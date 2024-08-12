// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ShortcutMapOverlayWidget.h"

#include <QDebug>

#ifdef _DEBUG
    #define SHORTCUT_MAP_OVERLAY_WIDGET_VERBOSE
#endif

namespace mv::gui
{

ShortcutMapOverlayWidget::ShortcutMapOverlayWidget(QWidget* parent) :
    OverlayWidget(parent),
    _widgetOverlayer(this, this, parent),
    _widgetFader(this, this, 1.f, 0.f, 1.f, 120, 60)
{
}

mv::util::WidgetFader& ShortcutMapOverlayWidget::getWidgetFader()
{
    return _widgetFader;
}

}
