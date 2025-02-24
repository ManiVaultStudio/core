// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ViewPluginOverlayWidget.h"

#include "ViewPlugin.h"

#include <QDebug>

#ifdef _DEBUG
    #define VIEW_PLUGIN_OVERLAY_WIDGET_VERBOSE
#endif

using namespace mv::util;

namespace mv::gui
{
ViewPluginOverlayWidget::CloseLabel::CloseLabel(ViewPluginOverlayWidget* ViewPluginOverlayWidget) :
    QLabel(ViewPluginOverlayWidget),
    _viewPluginOverlayWidget(ViewPluginOverlayWidget)
{
    setGraphicsEffect(&_opacityEffect);

    _opacityEffect.setOpacity(.5f);
}

void ViewPluginOverlayWidget::CloseLabel::enterEvent(QEnterEvent* event)
{
    QLabel::enterEvent(event);

    _opacityEffect.setOpacity(1.f);
}

void ViewPluginOverlayWidget::CloseLabel::leaveEvent(QEvent* event)
{
    QLabel::leaveEvent(event);

    _opacityEffect.setOpacity(.5f);
}

void ViewPluginOverlayWidget::CloseLabel::mousePressEvent(QMouseEvent* event)
{
    QLabel::mousePressEvent(event);

    _viewPluginOverlayWidget->deleteLater();
}

ViewPluginOverlayWidget::ViewPluginOverlayWidget(plugin::ViewPlugin* viewPlugin) :
    OverlayWidget(&viewPlugin->getWidget()),
    _viewPlugin(viewPlugin),
    _closeIconLabel(this)
{
    setAutoFillBackground(true);

    _mainLayout.setSpacing(10);

    _mainLayout.addLayout(&_toolbarLayout);

    setLayout(&_mainLayout);

    _toolbarLayout.addStretch(1);
    _toolbarLayout.addWidget(&_closeIconLabel);

    _closeIconLabel.setStyleSheet("opacity: 0.5");
    _closeIconLabel.setPixmap(QIcon(StyledIcon("xmark").withColor(QColor(0, 0, 0, 150))).pixmap(QSize(14, 14)));
}

QVBoxLayout& ViewPluginOverlayWidget::getMainLayout()
{
    return _mainLayout;
}

}
