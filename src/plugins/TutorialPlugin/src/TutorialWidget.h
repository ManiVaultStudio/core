// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <widgets/InfoOverlayWidget.h>

#include <QWidget>
#include <QVBoxLayout>
#include <QWebEngineView>

class TutorialPlugin;

/**
 * Tutorial widget class
 *
 * Displays tutorial context.
 *
 * @author Thomas Kroes
 */
class TutorialWidget : public QWidget
{
public:

    /**
     * Construct with pointer to owning \p tutorialPlugin and \p parent widget
     * @param tutorialPlugin Pointer to parent tutorial plugin
     * @param parent Pointer to parent widget
     */
    TutorialWidget(TutorialPlugin* tutorialPlugin, QWidget* parent = nullptr);

    /**
     * Set HTML text to \p htmlText
     * @param htmlText Text in HTML format
     * @param baseUrl Originating server
     */
    void setHtmlText(const QString& htmlText, const QUrl& baseUrl);

    /**
     * Get the base URL
     * @return Origination URL 
     */
    QUrl getBaseUrl() const;

private:
    TutorialPlugin*     _tutorialPlugin;        /** Pointer to parent tutorial plugin */
    QVBoxLayout         _layout;                /** Main layout */
    QWebEngineView      _webEngineView;         /** Web engine view in which the tutorial HTML is displayed */
    QUrl                _baseUrl;               /** Origination URL */
};
