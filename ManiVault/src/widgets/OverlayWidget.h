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
 * Overlays the parent widget with a custom widget (and synchronizes with its geometry) .
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

private:
    mv::util::WidgetOverlayer     _widgetOverlayer;      /** Utility for layering on top of the target widget */
};

}
