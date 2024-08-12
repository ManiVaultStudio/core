// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "widgets/OverlayWidget.h"

#include "util/WidgetFader.h"
#include "util/WidgetOverlayer.h"

namespace mv::gui
{

/**
 * Shortcut map overlay widget class
 *
 * Overlays the parent widget with a categorized shortcut map.
 *  
 * @author Thomas Kroes
 */
class CORE_EXPORT ShortcutMapOverlayWidget : public OverlayWidget
{
public:

    /**
     * Construct with \p parent
     * @param parent Pointer to parent widget
     */
    ShortcutMapOverlayWidget(QWidget* parent);

    /**
     * Get widget fader
     * @return Reference to widget fader
     */
    util::WidgetFader& getWidgetFader();

private:
    mv::util::WidgetOverlayer   _widgetOverlayer;   /** Utility for layering on top of the target widget */
    mv::util::WidgetFader       _widgetFader;       /** Widget fader for animating the widget opacity */
};

}
