// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PageWidget.h"

#include <QApplication>
#include <QDebug>
#include <QPainter>

PageWidget::PageWidget(const QString& title, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _pageHeaderWidget(title),
    _titleLabel(title),
    _backgroundImage(":/Images/StartPageBackground")
{
    setObjectName("PageWidget");
    setLayout(&_layout);
    setMinimumWidth(500);

    _layout.setContentsMargins(0, 0, 0, 0);
    _layout.setSpacing(0);

    _contentLayout.addWidget(&_pageHeaderWidget,  1);

    _titleLabel.setObjectName("Title");
    _titleLabel.setAutoFillBackground(true);
    _titleLabel.setStyleSheet(" \
        font-size: 18pt; \
        color: rgb(80, 80, 80); \
        padding-top: 10px; \
        padding-bottom: 0px; \
    ");

    _titleLabel.setAlignment(Qt::AlignCenter);

    _contentLayout.addWidget(&_titleLabel);

    _layout.addStretch(1);
    _layout.addLayout(&_contentLayout, 2);
    _layout.addStretch(1);

    updateCustomStyle();
}

bool PageWidget::event(QEvent* event)
{
    if (event->type() == QEvent::ApplicationPaletteChange)
        updateCustomStyle();

    return QWidget::event(event);
}

void PageWidget::paintEvent(QPaintEvent* paintEvent)
{
    QPainter painter(this);

    auto backgroundImage    = _backgroundImage.scaled(width(), height(), Qt::KeepAspectRatioByExpanding);
    auto centerOfWidget     = rect().center();
    auto pixmapRectangle    = backgroundImage.rect();

    pixmapRectangle.moveCenter(centerOfWidget);

    painter.drawPixmap(pixmapRectangle.topLeft(), QPixmap(backgroundImage));
}

void PageWidget::setWidgetBackgroundColorRole(QWidget* widget, const QPalette::ColorRole& colorRole)
{
    Q_ASSERT(widget != nullptr);
    
    // When the palette is changed during runtime, the system is updated before the widget
    // Therefore we translate the colorRole to a color using the global palette instead of the widget style option
    QString color = QApplication::palette().color(QPalette::Normal, colorRole).name();
    
    widget->setStyleSheet("QWidget#PageWidget, QWidget#PageHeaderWidget, QWidget#PageContentWidget, QLabel#Title { background-color: " + color + "}");
}

QVBoxLayout& PageWidget::getContentLayout()
{
    return _contentLayout;
}

void PageWidget::updateCustomStyle()
{
    PageWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);
}
