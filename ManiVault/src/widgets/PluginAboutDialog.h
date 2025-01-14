// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "MarkdownDocument.h"
#include "PluginMetaData.h"

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
    class Plugin;
}

namespace mv::gui
{

/**
 * Plugin about dialog class
 *
 * Show plugin about Markdown text
 *
 * @author Thomas Kroes
 */
class PluginAboutDialog : public QDialog
{
public:

    /**
     * Construct with pointer to source \p plugin and pointer to \p parent widget
     * @param pluginMetaData Plugin meta data
     * @param parent Pointer to parent widget (maybe nullptr)
     */
    PluginAboutDialog(const plugin::PluginMetaData& pluginMetaData, QWidget* parent = nullptr);

    /** Get preferred size */
    QSize sizeHint() const override {
        return { 600, 800 };
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

private:
    const plugin::PluginMetaData&   _pluginMetaData;        /** Plugin meta data */
    QScrollArea                     _textScrollArea;        /** Scroll area for the shortcut */
    QWidget                         _textWidget;            /** Widget with the shortcuts label */
    QVBoxLayout                     _textWidgetLayout;      /** Widget with the shortcuts label */
    QLabel                          _textBodyLabel;         /** Shortcut cheatsheet HTML */
    QWebChannel                     _markdownChannel;       /** Markdown web channel */
    QUrl                            _markdownUrl;           /** Location of the Markdown file */
    QWebEngineView                  _webEngineView;         /** Browser to show the Markdown in */
    QWebEnginePage                  _markdownPage;          /** Browser page to show the Markdown in */
    util::MarkdownDocument          _markdownDocument;      /** Document for synchronizing the Markdown text with the browser */
};

}
