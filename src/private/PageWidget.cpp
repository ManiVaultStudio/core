// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PageWidget.h"

#include <Application.h>

#include <QDebug>
#include <QPainter>

using namespace mv;
using namespace mv::util;
using namespace mv::gui;

PageWidget::PageWidget(const QString& title, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    Serializable(title),
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

    titleWidget->setAutoFillBackground(true);
    titleWidget->setAttribute(Qt::WidgetAttribute::WA_StyledBackground);
    titleWidget->setLayout(titleLayout);
    titleWidget->setBackgroundRole(QPalette::Window);

    _titleLabel.setAlignment(Qt::AlignCenter);
    _titleLabel.setStyleSheet(
        "font-size: 18pt;"
        "color: rgb(80, 80, 80);"
        "padding-top: 10px;"
        "padding-bottom: 0px;"
    );

    titleLayout->addWidget(&_titleLabel);

    _contentLayout.addWidget(titleWidget);

    _layout.addStretch(1);
    _layout.addLayout(&_contentLayout, 2);
    _layout.addStretch(1);

    setBackgroundRole(QPalette::Window);
    setAttribute(Qt::WA_NoSystemBackground, false);

    auto& brandingConfigurationAction = Application::current()->getConfigurationAction().getBrandingConfigurationAction();

    connect(&brandingConfigurationAction.getLogoAction(), &ImageAction::imageChanged, this, &PageWidget::updateLogo);
    connect(&brandingConfigurationAction.getFullNameAction(), &StringAction::stringChanged, this, &PageWidget::updateTitle);

    updateLogo();
    updateTitle();
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

PageHeaderWidget& PageWidget::getPageHeaderWidget()
{
    return _pageHeaderWidget;
}

QVBoxLayout& PageWidget::getContentLayout()
{
    return _contentLayout;
}

void PageWidget::updateLogo()
{
    _pageHeaderWidget.setPixmap(QPixmap::fromImage(Application::current()->getConfigurationAction().getBrandingConfigurationAction().getLogoAction().getImage()));
}

void PageWidget::updateTitle()
{
    _titleLabel.setText(Application::current()->getConfigurationAction().getBrandingConfigurationAction().getFullNameAction().getString());
}

