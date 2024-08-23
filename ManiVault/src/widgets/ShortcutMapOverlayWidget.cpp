// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ShortcutMapOverlayWidget.h"

#include <QDebug>

#ifdef _DEBUG
    #define SHORTCUT_MAP_OVERLAY_WIDGET_VERBOSE
#endif

using namespace mv::util;

namespace mv::gui
{

ShortcutMapOverlayWidget::ShortcutMapOverlayWidget(QWidget* source, const util::ShortcutMap& shortcutMap) :
    OverlayWidget(source),
    _shortcutMap(shortcutMap),
    _widgetFader(this, this, 0.f, 0.f, 1.0f, 250, 250),
    _closeAction(this, "Close")
{
    setAutoFillBackground(true);
    setStyleSheet("background-color: white;");

    _layout.addWidget(&_label);
    _layout.addStretch(1);
    _layout.addWidget(_closeAction.createWidget(this));

    setLayout(&_layout);

    const auto header = QString("<p><b>%1</b> shortcuts</p>").arg(_shortcutMap.getTitle());

    _label.setText(header);

    connect(&_closeAction, &TriggerAction::triggered, &_widgetFader, [this]() -> void {
        connect(&_widgetFader, &WidgetFader::fadedOut, this, &ShortcutMapOverlayWidget::deleteLater);
        getWidgetFader().fadeOut();
    });

    this->installEventFilter(this);
}

bool ShortcutMapOverlayWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Enter:
        case QEvent::Leave:
        case QEvent::MouseMove:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
            //qDebug() << __FUNCTION__ << event->type();
            event->ignore();
            return true;

        default:
            break;
    }

    return OverlayWidget::eventFilter(target, event);
}

mv::util::WidgetFader& ShortcutMapOverlayWidget::getWidgetFader()
{
    return _widgetFader;
}

void ShortcutMapOverlayWidget::showEvent(QShowEvent* event)
{
    //_widgetFader.fadeIn();

    OverlayWidget::showEvent(event);
}
}
