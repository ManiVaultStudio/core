// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "widgets/OverlayWidget.h"

#include "util/WidgetFader.h"
#include "util/WidgetOverlayer.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

namespace mv::plugin {
    class ViewPlugin;
}

namespace mv::gui
{

/**
 * View plugin learning center overlay widget class
 *
 * Overlays the source widget with a view plugin learning center icon popup
 *  
 * @author Thomas Kroes
 */
class ViewPluginLearningCenterOverlayWidget : public OverlayWidget
{
private:

    class PopupWidget : public QWidget {
    public:
        PopupWidget(QWidget* parent = nullptr);

    private:
        QLabel  _label;
    };

public:

    /**
     * Construct with pointer to \p source widget and pointer to \p viewPlugin
     * @param source Pointer to source widget
     * @param viewPlugin Pointer to the view plugin for which to create the overlay
     */
    ViewPluginLearningCenterOverlayWidget(QWidget* source, const plugin::ViewPlugin* viewPlugin);

    /**
     * Get widget fader
     * @return Reference to widget fader
     */
    util::WidgetFader& getWidgetFader();

private:
    const plugin::ViewPlugin*   _viewPlugin;        /** Pointer to the view plugin for which to create the overlay */
    mv::util::WidgetOverlayer   _widgetOverlayer;   /** Utility for layering on top of the target widget */
    mv::util::WidgetFader       _widgetFader;       /** Widget fader for animating the widget opacity */
    QHBoxLayout                 _horizontalLayout;  /** For horizontal alignment of the learning center icon */
    QVBoxLayout                 _verticalLayout;    /** For Vertical alignment of the learning center icon */
    PopupWidget                 _popupWidget;       /** Icon with popup */
};

}
