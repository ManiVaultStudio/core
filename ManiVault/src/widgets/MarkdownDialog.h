// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QDialog>

#include "MarkdownDocument.h"

#include "util/FileDownloader.h"

#include <QWebEngineView>
#include <QWebEnginePage>

namespace mv::util {

/**
 * Plugin read me dialog class
 * 
 * Dialog class for displaying plugin read me markdown content
 * 
 * @author Thomas Kroes
 */
class MarkdownDialog : public QDialog
{
public:

    /**
     * Construct with \p readmeUrl and pointer to \p parent widget
     * @param markdownUrl URL of the plugin markdown URL
     * @param parent Pointer to parent widget
     */
    MarkdownDialog(const QUrl& markdownUrl, QWidget* parent = nullptr);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(800, 600);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

private:
    QUrl                        _markdownUrl;           /** Location of the markdown file */
    QWebEngineView              _webEngineView;         /** Browser to show the markdown in */
    QWebEnginePage              _markdownPage;          /** Browser page to show the markdown in */
    mv::util::FileDownloader    _fileDownloader;        /** For downloading the markdown file */
    MarkdownDocument            _markdownDocument;      /** Document for synchronizing the markdown text with the browser */
};

}