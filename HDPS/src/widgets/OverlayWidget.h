// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "util/WidgetOverlayer.h"

#include <QWidget>

namespace hdps::gui
{

/**
 * Overlay widget class
 *
 * Overlays the parent widget with a custom widget (and synchronizes with its geometry) .
 *  
 * @author Thomas Kroes
 */
class OverlayWidget : public QWidget
{
public:

    /**
     * Construct with \p parent
     * @param parent Pointer to parent widget
     * @param initialOpacity Opacity at initialization
     */
    OverlayWidget(QWidget* parent, float initialOpacity = 1.0f);

    /**
     * Get the utility class for overlaying the widget
     * @return Widget overlayer
     */
    hdps::util::WidgetOverlayer& getWidgetOverlayer();

private:
    hdps::util::WidgetOverlayer     _widgetOverlayer;      /** Utility for layering on top of the target widget */
};

}
