// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TutorialWidget.h"

#include <Application.h>

#include "TutorialPlugin.h"

#include <QDebug>
#include <QWebEngineSettings>

namespace mv::gui
{
    class InfoOverlayWidget;
}

using namespace mv;
using namespace mv::gui;
using namespace mv::plugin;

TutorialWidget::TutorialWidget(TutorialPlugin* tutorialPlugin, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _tutorialPlugin(tutorialPlugin),
    _infoOverlayWidget(&_webEngineView, Application::getIconFont("FontAwesome").getIcon("eye-dropper"), "No samples view", "There is currently no samples view available...")
{
    setAutoFillBackground(true);
    setLayout(&_layout);

    auto& widgetFader = _infoOverlayWidget.getWidgetFader();

    widgetFader.setOpacity(0.0f);
    widgetFader.setMaximumOpacity(0.5f);
    widgetFader.setFadeInDuration(100);
    widgetFader.setFadeOutDuration(300);

    _layout.setContentsMargins(0, 0, 0, 0);
    _layout.addWidget(&_webEngineView);

    _infoOverlayWidget.show();
}

void TutorialWidget::setHtmlText(const QString& htmlText, const QUrl& baseUrl)
{
    QString sanitizedHtmltext = QString(R"(
    <html>
        <head>
            <link rel="stylesheet" type="text/css" href="/assets/css/custom.css">
            <link rel="stylesheet" type="text/css" href="/assets/bootstrap/css/bootstrap.min.css">
            <style>
                a {
                    text-decoration: none; /* Remove underline */
                    color: inherit; /* Inherit text color from parent */
                }
                container {
                    margin: 10px;
                }
            </style>
        </head>
        <body>
            <main class="container">%1</main>
        </body>
    </html>
    )").arg(htmlText);

    _webEngineView.setHtml(sanitizedHtmltext, baseUrl);
}

InfoOverlayWidget& TutorialWidget::getInfoOverlayWidget()
{
    return _infoOverlayWidget;
}
