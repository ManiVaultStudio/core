// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SampleScopeWidget.h"

#include <Application.h>

#include <util/StyledIcon.h>

#include "SampleScopePlugin.h"

#include <QDebug>

using namespace mv;
using namespace mv::gui;
using namespace mv::plugin;
using namespace mv::util;

SampleScopeWidget::SampleScopeWidget(SampleScopePlugin* sampleScopePlugin, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _sampleScopePlugin(sampleScopePlugin),
    _noSamplesOverlayWidget(&_textHtmlView, StyledIcon("eye-dropper"), "No samples view", "There is currently no samples view available...")
{
    setAutoFillBackground(true);
    setLayout(&_layout);

    auto& widgetFader = _noSamplesOverlayWidget.getWidgetFader();

    widgetFader.setOpacity(1.0f);
    widgetFader.setMaximumOpacity(0.5f);
    widgetFader.setFadeInDuration(100);
    widgetFader.setFadeOutDuration(300);

    _layout.setContentsMargins(0, 0, 0, 0);
    _layout.addWidget(&_textHtmlView);

    _noSamplesOverlayWidget.show();
}

void SampleScopeWidget::setHtmlText(const QString& htmlText)
{
    _textHtmlView.setHtml(htmlText);
}

InfoOverlayWidget& SampleScopeWidget::getNoSamplesOverlayWidget()
{
    return _noSamplesOverlayWidget;
}
