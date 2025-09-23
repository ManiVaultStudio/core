// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "MarkdownDialog.h"

#include <Application.h>
#include <CoreInterface.h>

#include <QVBoxLayout>
#include <QWebChannel>
#include <QDesktopServices>
#include <QFutureWatcher>

#ifdef _DEBUG
    #define MARKDOWN_DIALOG_VERBOSE
#endif

using namespace mv::gui;

namespace mv::util {

MarkdownDialog::MarkdownDialog(const QUrl& markdownUrl, QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _markdownUrl(markdownUrl)
{
    setWindowIcon(StyledIcon("book"));
    setModal(true);

    auto channel = new QWebChannel(this);

    channel->registerObject(QStringLiteral("content"), &_markdownDocument);

    _markdownPage.setWebChannel(channel);

    connect(&_markdownPage, &QWebEnginePage::loadFinished, this, [this](bool ok) -> void {
        auto future     = FileDownloader::downloadToByteArrayAsync(_markdownUrl);
        auto watcher    = new QFutureWatcher<QByteArray>(this);

        connect(watcher, &QFutureWatcher<QByteArray>::finished, watcher, [this, future, watcher]() {
            try {
                const auto& data = future.result();

#ifdef MARKDOWN_DIALOG_VERBOSE
                qDebug() << _markdownUrl.toString() << "downloaded (" << data.size() << "bytes)";
#endif

                if (!data.isEmpty())
                    _markdownDocument.setText(data);
                else
                    _markdownDocument.setText(QString("# Unable to display markdown file\n*%1* not found").arg(_markdownUrl.toString()));
            }
            catch (std::exception& exception) {
                qWarning() << "Download markdown failed: " << exception.what();
            }

            watcher->deleteLater();
        });

        watcher->setFuture(future);
    });

    _webEngineView.setPage(&_markdownPage);
    _webEngineView.load(QUrl("qrc:/HTML/Markdown"));

    auto layout = new QVBoxLayout();

    layout->addWidget(&_webEngineView);

    auto bottomLayout = new QHBoxLayout();

    bottomLayout->addStretch(1);

    layout->addLayout(bottomLayout);

    setLayout(layout);
}

bool MarkdownWebEnginePage::acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame)
{
    if (type == QWebEnginePage::NavigationTypeLinkClicked) {
        QDesktopServices::openUrl(url);
        return false;
    }

    return QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);
}

MarkdownWebEnginePage::MarkdownWebEnginePage(QObject* parent /*= nullptr*/) :
    QWebEnginePage(parent)
{

}

}
