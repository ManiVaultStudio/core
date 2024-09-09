// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "widgets/OverlayWidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QGraphicsOpacityEffect>

namespace mv::util {
    class ShortcutMap;
}

namespace mv::plugin {
    class ViewPlugin;
}

namespace mv::gui
{

/**
 * View plugin widget class
 *
 * Overlays the view plugin widget with an overlay widget
 *  
 * @author Thomas Kroes
 */
class ViewPluginOverlayWidget : public OverlayWidget
{
private:

    /** Custom close label with mouse hover opacity */
    class CloseLabel : public QLabel
    {
    public:

        /**
         * Construct with pointer to parent \p viewPluginOverlayWidget
         * @param viewPluginOverlayWidget Pointer to parent view plugin overlay widget 
         */
        CloseLabel(ViewPluginOverlayWidget* viewPluginOverlayWidget);

        /**
         * Invoked when the mouse enters the toolbar item widget
         * @param event Pointer to enter event
         */
        void enterEvent(QEnterEvent* event) override;

        /**
         * Invoked when the mouse leaves the toolbar item widget
         * @param event Pointer to event
         */
        void leaveEvent(QEvent* event) override;

        /**
         * Invoked when the mouse button is pressed
         * @param event Pointer to mouse event
         */
        void mousePressEvent(QMouseEvent* event) override;

    private:
        ViewPluginOverlayWidget*    _viewPluginOverlayWidget;   /** Pointer to parent view plugin overlay widget */
        QGraphicsOpacityEffect      _opacityEffect;             /** Effect for modulating label opacity */
    };

public:

    /**
     * Construct with pointer to source \p view plugin
     * @param viewPlugin Pointer to source view plugin
     */
    ViewPluginOverlayWidget(plugin::ViewPlugin* viewPlugin);

    /**
     * Get main layout
     * @return Reference to main layout
     */
    QVBoxLayout& getMainLayout();

private:
    plugin::ViewPlugin*     _viewPlugin;            /** Pointer to source view plugin */
    QVBoxLayout             _mainLayout;            /** Main layout */
    QHBoxLayout             _toolbarLayout;         /** Layout for the toolbar */
    CloseLabel              _closeIconLabel;        /** Label for close icon */
};

}
