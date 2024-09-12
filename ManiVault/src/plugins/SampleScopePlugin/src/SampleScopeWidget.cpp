// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SampleScopeWidget.h"

#include <Application.h>

#include "SampleScopePlugin.h"

#include <QDebug>

using namespace mv;
using namespace mv::gui;
using namespace mv::plugin;

SampleScopeWidget::SampleScopeWidget(SampleScopePlugin* sampleScopePlugin, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _sampleScopePlugin(sampleScopePlugin),
    _noSamplesOverlayWidget(&_textScrollArea, Application::getIconFont("FontAwesome").getIcon("eye-dropper"), "No samples view", "There is currently no samples view available...")
{
    setAutoFillBackground(true);
    setLayout(&_layout);

    auto& widgetFader = _noSamplesOverlayWidget.getWidgetFader();

    widgetFader.setOpacity(0.0f);
    widgetFader.setMaximumOpacity(0.5f);
    widgetFader.setFadeInDuration(100);
    widgetFader.setFadeOutDuration(300);

    _layout.setContentsMargins(0, 0, 0, 0);
    _layout.addWidget(&_textScrollArea);

    _textScrollArea.setWidgetResizable(true);
    _textScrollArea.setWidget(&_textWidget);
    _textScrollArea.setObjectName("Shortcuts");
    //_textScrollArea.setStyleSheet("QScrollArea#Shortcuts { border: none; }");
    _textScrollArea.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    _textWidgetLayout.setContentsMargins(4, 4, 4, 4);
    _textWidgetLayout.addWidget(&_textBodyLabel);
    _textWidgetLayout.setAlignment(Qt::AlignTop);

    _textWidget.setLayout(&_textWidgetLayout);
    
    _textBodyLabel.setWordWrap(true);

    _noSamplesOverlayWidget.show();
}

void SampleScopeWidget::setHtmlText(const QString& htmlText)
{
    _textBodyLabel.setText(htmlText);
}

InfoOverlayWidget& SampleScopeWidget::getNoSamplesOverlayWidget()
{
    return _noSamplesOverlayWidget;
}
