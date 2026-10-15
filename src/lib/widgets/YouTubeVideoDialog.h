// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QDialog>

#include <QWebEngineView>
#include <QWebEnginePage>

namespace mv::gui {

/**
 * YouTube video dialog class
 * 
 * Dialog class for displaying a YouTube video (WIP)
 * 
 * @author Thomas Kroes
 */
class CORE_EXPORT YouTubeVideoDialog : public QDialog
{
public:

    /**
     * Construct with \p videoId and pointer to \p parent widget
     * @param videoId Globally unique identifier of the video
     * @param parent Pointer to parent widget
     */
    YouTubeVideoDialog(const QString& videoId, QWidget* parent = nullptr);

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(800, 600);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

    /**
     * Create YouTube video dialog and play \p videoUrl
     * @param videoId Globally unique identifier of the video
     */
    static void play(const QString& videoId);

private:
    QString             _videoId;           /** Globally unique identifier of the video */
    QWebEngineView      _webEngineView;     /** Browser to show the video in */
    QWebEnginePage      _videoPage;         /** Browser page to show the video in */
};

}