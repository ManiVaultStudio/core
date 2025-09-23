// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TutorialWidget.h"

#include <QCoreApplication>

#include "TutorialPlugin.h"

#include <QDebug>
#include <QWebEngineView>

namespace mv::gui
{
    class InfoOverlayWidget;
}

using namespace mv;
using namespace mv::gui;
using namespace mv::plugin;
using namespace mv::util;

TutorialWidget::TutorialWidget(TutorialPlugin* tutorialPlugin, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _tutorialPlugin(tutorialPlugin)
{
    setAutoFillBackground(true);

    _layout.setContentsMargins(0, 0, 0, 0);
    _layout.addWidget(&_webEngineView);

    setLayout(&_layout);
}

void TutorialWidget::setHtmlText(const QString& htmlText, const QUrl& baseUrl)
{
    QString sanitizedHtmltext = htmlText;

    if (const auto tutorial = _tutorialPlugin->getCurrentTutorial()) {
        QStringList tags;


        for (const auto& tag : tutorial->getTags())
            tags << QString("<div style='display: inline; background-color: lightgrey; padding-left: 5px; padding-right: 5px; padding-top: 2px; padding-bottom: 2px; border-radius: 4px; font-size: 8pt; margin-right: 4px;'>%1</div>").arg(tag);

        sanitizedHtmltext = QString(R"(
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
                <main class="container">
                    <h1 class="display-6 fw-bold">%1</h1>
                    <p>%2</p>
                    %3
                </main>
            </body>
        </html>
        )").arg(_tutorialPlugin->getTutorialPickerAction().getCurrentText(), tags.join(""), htmlText);

        sanitizedHtmltext = sanitizedHtmltext.replace(QRegularExpression("This tutorial requires a starter project.*?if you need it"), "");
    }
    
    _webEngineView.setHtml(sanitizedHtmltext, baseUrl);
    _webEngineView.show();

    _baseUrl = baseUrl;
}

QUrl TutorialWidget::getBaseUrl() const
{
    return _baseUrl;
}
