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

ShortcutMapOverlayWidget::ShortcutMapOverlayWidget(QWidget* source, const util::ShortcutMap& shortcutMap) :
    OverlayWidget(source),
    _shortcutMap(shortcutMap),
    _widgetFader(this, this, 1.f, 0.f, 1.f, 120, 60)
{
    setAutoFillBackground(true);

    _layout.addWidget(&_label);

    setLayout(&_layout);
}

mv::util::WidgetFader& ShortcutMapOverlayWidget::getWidgetFader()
{
    return _widgetFader;
}

void ShortcutMapOverlayWidget::showEvent(QShowEvent* event)
{
    _widgetFader.fadeIn();

    OverlayWidget::showEvent(event);
}

void ShortcutMapOverlayWidget::hideEvent(QHideEvent* event)
{
    _widgetFader.fadeOut();

    OverlayWidget::hideEvent(event);
}

}
