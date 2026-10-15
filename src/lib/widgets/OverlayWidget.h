// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "util/WidgetOverlayer.h"

#include <QWidget>

class QMouseMoveEvent;

namespace mv::gui
{

/**
 * Overlay widget class
 *
 * Overlays the target widget with an overlay widget and keeps its geometry in sync with the target widget.
 *
 * By default, the overlay widget and its children are immune to mouse events (similar to the Qt widget
 * attribute Qt::WA_TransparentForMouseEvents). To enable/disable child widget mouse events, call
 * OverlayWidget::addMouseEventReceiverWidget(...) or OverlayWidget::removeMouseEventReceiverWidget(...)
 * respectively.
 *
 * For more information, refer to the mv::util::WidgetOverlayer class.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT OverlayWidget : public QWidget
{
public:

    /**
     * Construct with pointer to \p target widget and initial opacity
     * @param target Pointer to target widget (used to synchronize the geometry with)
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
    virtual void addMouseEventReceiverWidget(QWidget* mouseEventReceiverWidget) final;

    /**
     * Remove \p mouseEventReceiverWidget
     * @param mouseEventReceiverWidget Pointer to mouse event receiver widget that should be removed
     */
    virtual void removeMouseEventReceiverWidget(QWidget* mouseEventReceiverWidget) final;

protected:

    /**
     * Update the mask region when the overlay widget changes size
     * @param event Pointer to resize event that occurred
     */
    void resizeEvent(QResizeEvent* event) override;

public:

    /** Update widget mask to selectively process mouse events */
    void updateMask();

private:
    mv::util::WidgetOverlayer     _widgetOverlayer;      /** Utility for layering on top of the target widget */
};

}
