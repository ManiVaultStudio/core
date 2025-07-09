// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <widgets/InfoOverlayWidget.h>

#include <ViewPlugin.h>

#include <QWidget>
#include <QVBoxLayout>
#include <QWebEngineView>
#include <QGraphicsView>
#include <QGraphicsScene>

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

    /** Initialize the widget */
    void initialize();

    /**
     * Set html text
     * @param html Text in HTML format
     */
    void setViewHtml(const QString& html);

    /**
     * Set widget to \p widget
     * @param widget Pointer to widget
     */
    void setViewWidget(const QWidget* widget);

    /**
     * Get no samples overlay widget
     * @return No samples overlay widget
     */
    mv::gui::InfoOverlayWidget& getNoSamplesOverlayWidget();

    /**
     * Resize the SampleScopeWidget#_widgetViewScene when the SampleScopeWidget size changes
     * @param event Pointer to resize event
     */
    void resizeEvent(QResizeEvent* event) override;

private:

    /** Toggles the visibility of the widgets */
    void updateVisibility();

    /** Updates the size of the widget proxy */
    void updateViewWidgetProxySize() const;
    
private:
    SampleScopePlugin*              _sampleScopePlugin;                 /** Pointer to parent sample scope plugin */
    QVBoxLayout                     _mainLayout;                        /** Main layout */  
    QWidget                         _viewsWidget;                       /** Widget containing the views */
    QVBoxLayout                     _viewsWidgetLayout;                 /** Layout of the views widget */
    QWebEngineView                  _htmlView;                          /** Web engine view for displaying HTML content */
    QGraphicsView                   _widgetView;                        /** Graphics view in which the widget is displayed */
    QGraphicsScene                  _widgetViewScene;                   /** Graphics scene in which the widget is displayed */
    QGraphicsProxyWidget*           _proxyWidget;                       /** Proxy widget for the widget */
    QWidget*                        _currentViewWidget;                 /** Pointer to the current view widget */
    mv::gui::InfoOverlayWidget      _noSamplesOverlayWidget;            /** Overlay widget with a message saying there are no samples available */
    mv::plugin::ViewPlugin*         _currentViewPlugin;                 /** Pointer to the current view plugin */
    mv::plugin::ViewPlugin*         _previousViewPlugin = nullptr;      /** Pointer to the previous view plugin */
};
