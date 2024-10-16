// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "MarkdownDocument.h"

#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebChannel>

namespace mv::util {
    class ShortcutMap;
}

namespace mv::plugin {
    class ViewPlugin;
}

namespace mv::gui
{

/**
 * View plugin description dialog class
 *
 * Dialog with a view plugin description
 *
 * @author Thomas Kroes
 */
class ViewPluginDescriptionDialog : public QDialog
{
public:

    /**
     * Construct with pointer to source \p view plugin and pointer to \p parent widget
     * @param viewPlugin Pointer to source view plugin
     * @param parent Pointer to parent widget (maybe nullptr)
     */
    ViewPluginDescriptionDialog(mv::plugin::ViewPlugin* viewPlugin, QWidget* parent = nullptr);

    /** Get preferred size */
    QSize sizeHint() const override {
        return { 600, 800 };
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

private:
    QScrollArea                 _textScrollArea;        /** Scroll area for the shortcut */
    QWidget                     _textWidget;            /** Widget with the shortcuts label */
    QVBoxLayout                 _textWidgetLayout;      /** Widget with the shortcuts label */
    QLabel                      _textBodyLabel;         /** Shortcut cheatsheet HTML */
    QWebChannel                 _markdownChannel;       /** Markdown web channel */
    QUrl                        _markdownUrl;           /** Location of the Markdown file */
    QWebEngineView              _webEngineView;         /** Browser to show the Markdown in */
    QWebEnginePage              _markdownPage;          /** Browser page to show the Markdown in */
    util::MarkdownDocument      _markdownDocument;      /** Document for synchronizing the Markdown text with the browser */
};

}
