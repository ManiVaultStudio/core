// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "util/WidgetOverlayer.h"

#include <QWidget>

namespace mv::gui
{

/**
 * Overlay widget class
 *
 * Overlays the parent widget with a custom widget (and synchronizes with its geometry).
 *
 * By default, the overlay widget and its children are immune to mouse events. This can be
 * circumvented by adding explicitly adding a widget that should receive mouse events, see
 * OverlayWidget::addMouseEventReceiverWidget(...) and WidgetOverlayer::addMouseEventReceiverWidget().
 *  
 * @author Thomas Kroes
 */
class CORE_EXPORT OverlayWidget : public QWidget
{
public:

    /**
     * Construct with pointer to \p target widget and initial opacity
     * @param target Pointer to target widget
     * @param initialOpacity Opacity at initialization
     */
    OverlayWidget(QWidget* target, float initialOpacity = 1.0f);

    /**
     * Get the utility class for overlaying the widget
     * @return Widget overlayer
     */
    mv::util::WidgetOverlayer& getWidgetOverlayer();

    /**
     * Add \p mouseEventReceiverWidget
     * @param mouseEventReceiverWidget Pointer to mouse event receiver widget that should be added
     */
    void addMouseEventReceiverWidget(QWidget* mouseEventReceiverWidget);

    /**
     * Remove \p mouseEventReceiverWidget
     * @param mouseEventReceiverWidget Pointer to mouse event receiver widget that should be removed
     */
    void removeMouseEventReceiverWidget(QWidget* mouseEventReceiverWidget);

protected:
    //bool event(QEvent* event) override;
    //void enterEvent(QEnterEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

    void propagateMouseEvent(QMouseEvent* event);
    //bool eventFilter(QObject* watched, QEvent* event) override;
private:
    mv::util::WidgetOverlayer     _widgetOverlayer;      /** Utility for layering on top of the target widget */
};

}
