// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StartPageHeaderWidget.h"
#include "StartPageWidget.h"

#include <QDebug>
#include <QResizeEvent>
#include <Application.h>

#include <algorithm>

using namespace hdps;
using namespace hdps::gui;

StartPageHeaderWidget::StartPageHeaderWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _layout(),
    _headerLabel(),
    _iconName(":/Images/AppBackground256"),
    _previousHeight(-1)
{
    setLayout(&_layout);
    setAutoFillBackground(true);

    setMinimumHeight(150);

    const int pixelRatio = devicePixelRatio();

    if (pixelRatio > 1)
        _iconName = ":/Images/AppBackground512";

    if (pixelRatio > 2)
        _iconName = ":/Images/AppBackground1024";

    _headerLabel.setPixmap(QPixmap(_iconName).scaled(256, 256));
    _headerLabel.setAlignment(Qt::AlignCenter);

    _layout.setContentsMargins(50, 25, 50, 0);
    _layout.addWidget(&_headerLabel);
    
    updateCustomStyle();
    connect(qApp, &QApplication::paletteChanged, this, &StartPageHeaderWidget::updateCustomStyle);
}

void StartPageHeaderWidget::resizeEvent(QResizeEvent* event)
{
    // resize icon when widget is resized
    resizeIcon(event->size());
}

void StartPageHeaderWidget::showEvent(QShowEvent* event)
{
    // resize icon when widget is first opened
    resizeIcon(size());
}

void StartPageHeaderWidget::resizeIcon(const QSize& newSize)
{
    // only update when visible and if the window height changed, i.e. ignore width changes
    if (isVisible() && newSize.height() != _previousHeight)
    {
        float fracHeight = newSize.height() / 296.0f; // 296 = 2 * top margin + default pixmap size
        float scale = std::clamp(fracHeight, 0.1f, 1.0f);

        _headerLabel.setPixmap(QPixmap(_iconName).scaled(scale * 256, scale * 256));

        _previousHeight = newSize.height();
    }
}

void StartPageHeaderWidget::updateCustomStyle()
{
    // update custome style settings
    StartPageWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);
}
