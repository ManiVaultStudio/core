// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "MarkdownDialog.h"

#include <Application.h>
#include <CoreInterface.h>

#include <QVBoxLayout>
#include <QWebChannel>

#ifdef _DEBUG
    #define MARKDOWN_DIALOG_VERBOSE
#endif

using namespace mv::gui;

namespace mv::util {

MarkdownDialog::MarkdownDialog(const QUrl& markdownUrl, QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _markdownUrl(markdownUrl),
    _webEngineView(),
    _markdownPage(),
    _fileDownloader(),
    _markdownDocument()
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("book"));
    setModal(true);

    _webEngineView.setPage(&_markdownPage);
    _webEngineView.load(QUrl("qrc:/HTML/MarkdownReadme"));

    connect(&_fileDownloader, &FileDownloader::downloaded, this, [this]() -> void {
        const auto markdown = QString(_fileDownloader.downloadedData());

        if (!markdown.isEmpty())
            _markdownDocument.setText(markdown);
        else
            _markdownDocument.setText(QString("# Unable to display markdown file\n*%1* not found").arg(_markdownUrl.path()));
    });

    _fileDownloader.download(_markdownUrl);

    auto channel = new QWebChannel(this);
    
    channel->registerObject(QStringLiteral("content"), &_markdownDocument);
    
    _markdownPage.setWebChannel(channel);

    auto layout = new QVBoxLayout();

    layout->addWidget(&_webEngineView);

    auto bottomLayout = new QHBoxLayout();

    bottomLayout->addStretch(1);

    layout->addLayout(bottomLayout);

    setLayout(layout);

    //connect(&_okAction, &TriggerAction::triggered, this, &ProjectSettingsDialog::accept);
}

}