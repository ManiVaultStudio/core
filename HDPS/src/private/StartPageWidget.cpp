// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartPageWidget.h"

#include <Application.h>
#include <CoreInterface.h>

#include <widgets/Divider.h>

#include <QDebug>
#include <QFileInfo>
#include <QStyleOption>
#include <QScrollBar>
#include <QResizeEvent>
#include <QPainter>

using namespace mv;
using namespace mv::gui;

StartPageWidget::StartPageWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _layout(),
    _centerColumnLayout(),
    _startPageContentWidget(this),
    _backgroundImage(":/Images/StartPageBackground")
{
    setLayout(&_layout);
    setMinimumWidth(500);

    _layout.setContentsMargins(0, 0, 0, 0);
    _layout.setSpacing(0);

    _centerColumnLayout.addWidget(&_startPageHeaderWidget,  1);
    _centerColumnLayout.addWidget(&_startPageContentWidget, 3); // give content more space than header

    _layout.addStretch(1);
    _layout.addLayout(&_centerColumnLayout, 2);
    _layout.addStretch(1);
}

void StartPageWidget::paintEvent(QPaintEvent* paintEvent)
{
    QPainter painter(this);

    auto backgroundImage    = _backgroundImage.scaled(width(), height(), Qt::KeepAspectRatioByExpanding);
    auto centerOfWidget     = rect().center();
    auto pixmapRectangle    = backgroundImage.rect();

    pixmapRectangle.moveCenter(centerOfWidget);

    painter.drawPixmap(pixmapRectangle.topLeft(), QPixmap(backgroundImage));
}

void StartPageWidget::showEvent(QShowEvent* showEvent)
{
    _startPageContentWidget.updateActions();
}

void StartPageWidget::setWidgetBackgroundColorRole(QWidget* widget, const QPalette::ColorRole& colorRole)
{
    Q_ASSERT(widget != nullptr);
    
    // When the palette is changed during runtime, the system is updated before the widget
    // Therefore we translate the colorRole to a color using the global palette instead of the widget style option
    QString color = QApplication::palette().color(QPalette::Normal, colorRole).name();
    
    //widget->setPalette(palette);
    widget->setStyleSheet("QWidget#StartPageHeaderWidget, QWidget#StartPageContentWidget { background-color: " + color + "}");
}
