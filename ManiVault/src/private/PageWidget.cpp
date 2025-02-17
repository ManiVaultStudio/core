// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PageWidget.h"

#include <QDebug>
#include <QPainter>

using namespace mv;

PageWidget::PageWidget(const QString& title, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _pageHeaderWidget(title),
    _backgroundImage(":/Images/StartPageBackground")
{
    setObjectName("PageWidget");
    setLayout(&_layout);
    setMinimumWidth(500);
    setAutoFillBackground(true);

    _layout.setContentsMargins(0, 0, 0, 0);
    _layout.setSpacing(0);

    _contentLayout.addWidget(&_pageHeaderWidget,  1);

    auto titleWidget    = new QWidget();
    auto titleLayout    = new QVBoxLayout();
    auto titleLabel     = new QLabel(title);

    titleWidget->setAutoFillBackground(true);
    titleWidget->setAttribute(Qt::WidgetAttribute::WA_StyledBackground);
    titleWidget->setLayout(titleLayout);
    titleWidget->setBackgroundRole(QPalette::Window);

    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "font-size: 18pt;"
        "color: rgb(80, 80, 80);"
        "padding-top: 10px;"
        "padding-bottom: 0px;"
    );

    titleLayout->addWidget(titleLabel);

    _contentLayout.addWidget(titleWidget);

    _layout.addStretch(1);
    _layout.addLayout(&_contentLayout, 2);
    _layout.addStretch(1);

    setBackgroundRole(QPalette::Window);
    setAttribute(Qt::WA_NoSystemBackground, false);
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

QVBoxLayout& PageWidget::getContentLayout()
{
    return _contentLayout;
}

