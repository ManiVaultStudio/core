// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <widgets/InfoOverlayWidget.h>

#include <QWidget>
#include <QVBoxLayout>
#include <QWebEngineView>

class SampleScopePlugin;

/**
 * Sample scope widget class
 *
 * Widget class for exploring a sample context.
 *
 * @author Thomas Kroes
 */
class SampleScopeWidget : public QWidget
{
public:

    /**
     * Construct with pointer to owning \p sampleScopePlugin and \p parent widget
     * @param sampleScopePlugin Pointer to parent sample scope plugin
     * @param parent Pointer to parent widget
     */
    SampleScopeWidget(SampleScopePlugin* sampleScopePlugin, QWidget* parent = nullptr);

    /**
     * Set text
     * @param htmlText Text in HTML format
     */
    void setHtmlText(const QString& htmlText);

    /**
     * Get no samples overlay widget
     * @return No samples overlay widget
     */
    mv::gui::InfoOverlayWidget& getNoSamplesOverlayWidget();

private:
    SampleScopePlugin*              _sampleScopePlugin;         /** Pointer to parent sample scope plugin */
    QVBoxLayout                     _layout;                    /** Main layout */
    QWebEngineView                  _textHtmlView;              /** Web engine view in which the HTML is displayed */
    mv::gui::InfoOverlayWidget      _noSamplesOverlayWidget;    /** Overlay widget with a message saying there are no samples available */
};
