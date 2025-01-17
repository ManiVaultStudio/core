// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginAboutDialog.h"

#include "Application.h"

#include "util/Miscellaneous.h"

#include <QDebug>
#include <QScrollBar>

#ifdef _DEBUG
    #define PLUGIN_ABOUT_DIALOG_VERBOSE
#endif

using namespace mv::util;

namespace mv::gui
{

PluginAboutDialog::PluginAboutDialog(const plugin::PluginMetadata& pluginMetaData, QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _pluginMetaData(pluginMetaData),
    _waitingOverlayWidget(this, Application::getIconFont("FontAwesome").getIcon("hourglass-start"), "Loading", "Please wait while the content is generated...")
{
    _waitingOverlayWidget.getWidgetFader().setOpacity(0.5f);

    setAutoFillBackground(true);
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("book-reader"));
    setWindowTitle(QString("%1 plugin").arg(_pluginMetaData.getGuiName()));

    auto layout = new QVBoxLayout();

    layout->addWidget(&_textScrollArea);

    setLayout(layout);

    layout->addWidget(&_textScrollArea);

    _textScrollArea.setWidgetResizable(true);
    _textScrollArea.setObjectName("About");
    _textScrollArea.setStyleSheet("QScrollArea#About { border: none; }");
    _textScrollArea.hide();

    _textScrollArea.setWidget(&_webEngineView);

    const auto aboutMarkdown = _pluginMetaData.getAboutMarkdown();

    if (!aboutMarkdown.isEmpty()) {
        _markdownChannel.registerObject(QStringLiteral("content"), &_markdownDocument);

        _markdownPage.setWebChannel(&_markdownChannel);
        
        connect(&_markdownPage, &QWebEnginePage::loadFinished, this, [this]() -> void {
            _textScrollArea.show();

            _markdownDocument.setText(_pluginMetaData.getAboutMarkdown());

            const auto appFont      = qApp->font();
            const auto fontFamily   = appFont.family();
            const auto fontSize     = appFont.pointSize();
            const auto fontColor    = qApp->palette().color(QPalette::Text);
            const auto colorHex     = fontColor.name();

            _markdownPage.runJavaScript(QString("document.body.style.fontFamily = '%1';").arg(fontFamily));
            _markdownPage.runJavaScript(QString("document.body.style.color = '%1';").arg(colorHex));
            _markdownPage.runJavaScript(QString("document.body.style.backgroundColor = '%1';").arg(getColorAsCssString(qApp->palette().window().color())));

            const auto setFontSizeJs = R"(
				var style = document.createElement('style');
				style.innerHTML = `body {
					font-size:)" + QString::number(fontSize) + R"(pt !important;
				}
				summary {
                    font-weight: bold;
                    margin-left: 0px;
                    margin-bottom: 10px;
				}
				summary:hover {
					text-decoration: underline;
					cursor: pointer;
				}`;
				document.head.appendChild(style);
			)";

            _markdownPage.runJavaScript(setFontSizeJs);
            _markdownPage.runJavaScript(QString("document.body.style.color = '%1';").arg(colorHex));

            QTimer::singleShot(250, [this]() -> void {
                _waitingOverlayWidget.getWidgetFader().fadeOut(350);
			});
		});

        _webEngineView.setPage(&_markdownPage);
        _webEngineView.load(QUrl("qrc:/HTML/Markdown"));
    } else {
        _textScrollArea.setWidget(&_textWidget);

        _textWidgetLayout.setContentsMargins(5, 0, 5, 0);
        _textWidgetLayout.setAlignment(Qt::AlignTop);
        _textWidgetLayout.addWidget(&_textBodyLabel);

        _textWidget.setLayout(&_textWidgetLayout);

        _textBodyLabel.setText(aboutMarkdown);
        _textBodyLabel.setWordWrap(true);
    }
}

}
