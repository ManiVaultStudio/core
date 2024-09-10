// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SampleScopeWidget.h"
#include "SampleScopePlugin.h"

#include <QDebug>

using namespace mv;
using namespace mv::gui;

SampleScopeWidget::SampleScopeWidget(SampleScopePlugin* sampleScopePlugin, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _sampleScopePlugin(sampleScopePlugin)
{
    setAutoFillBackground(true);
    setLayout(&_layout);

    _layout.setContentsMargins(0, 0, 0, 0);
    _layout.addWidget(&_textScrollArea);

    _textScrollArea.setWidgetResizable(true);
    _textScrollArea.setWidget(&_textWidget);
    _textScrollArea.setObjectName("Shortcuts");
    _textScrollArea.setStyleSheet("QScrollArea#Shortcuts { border: none; }");
    _textScrollArea.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    _textWidgetLayout.setContentsMargins(0, 0, 0, 0);
    _textWidgetLayout.addWidget(&_textBodyLabel);
    _textWidgetLayout.setAlignment(Qt::AlignTop);

    _textWidget.setLayout(&_textWidgetLayout);
    
    _textBodyLabel.setWordWrap(true);
}

void SampleScopeWidget::setHtmlText(const QString& htmlText)
{
    _textBodyLabel.setText(htmlText);
}
