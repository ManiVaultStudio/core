// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ViewPluginDescriptionOverlayWidget.h"

#include "Application.h"
#include "ViewPlugin.h"

#include <QDebug>
#include <QScrollBar>

#ifdef _DEBUG
    #define VIEW_PLUGIN_DESCRIPTION_OVERLAY_WIDGET_VERBOSE
#endif

using namespace mv::util;

namespace mv::gui
{

ViewPluginDescriptionOverlayWidget::ViewPluginDescriptionOverlayWidget(plugin::ViewPlugin* viewPlugin) :
    ViewPluginOverlayWidget(viewPlugin)
{
    setAutoFillBackground(true);

    getMainLayout().addLayout(&_headerLayout);
    getMainLayout().addWidget(&_textScrollArea);

    _headerLayout.setContentsMargins(5, 0, 5, 0);
    _headerLayout.setSpacing(10);

    _headerLayout.addWidget(&_headerIconLabel);
    _headerLayout.addWidget(&_headerTextLabel);
    _headerLayout.addStretch(1);

    _headerIconLabel.setPixmap(Application::getIconFont("FontAwesome").getIcon("book-reader").pixmap(QSize(24, 24)));
    _headerIconLabel.setStyleSheet("padding-top: 2px;");

    _headerTextLabel.setText(QString("<p style='font-size: 16pt;'><b>%1</b></p>").arg(viewPlugin->getLearningCenterAction().getPluginTitle()));

    _textScrollArea.setWidgetResizable(true);
    
    _textScrollArea.setObjectName("Shortcuts");
    _textScrollArea.setStyleSheet("QScrollArea#Shortcuts { border: none; }");
    _textScrollArea.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    const auto longDescriptionMarkdown = viewPlugin->getLearningCenterAction().getLongDescriptionMarkdown();

    if (!longDescriptionMarkdown.isEmpty()) {
        _markdownChannel.registerObject(QStringLiteral("content"), &_markdownDocument);

        _markdownPage.setWebChannel(&_markdownChannel);

        connect(&_markdownPage, &QWebEnginePage::loadFinished, this, [this, viewPlugin]() -> void {
            _markdownDocument.setText(viewPlugin->getLearningCenterAction().getLongDescriptionMarkdown());
		});

        _webEngineView.setPage(&_markdownPage);
        _webEngineView.load(QUrl("qrc:/HTML/MarkdownReadme"));

        _textScrollArea.setWidget(&_webEngineView);

    } else {
        const auto longDescription = viewPlugin->getLearningCenterAction().getLongDescription();

        _textScrollArea.setWidget(&_textWidget);

        _textWidgetLayout.setContentsMargins(5, 0, 5, 0);
        _textWidgetLayout.setAlignment(Qt::AlignTop);
        _textWidgetLayout.addWidget(&_textBodyLabel);

        _textWidget.setLayout(&_textWidgetLayout);

        _textBodyLabel.setText(longDescription);
        _textBodyLabel.setWordWrap(true);
    }
}

}
