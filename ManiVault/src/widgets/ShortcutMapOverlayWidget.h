// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "widgets/OverlayWidget.h"

#include "util/WidgetFader.h"
#include "util/WidgetOverlayer.h"

namespace mv::util {
    class ShortcutMap;
}

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
     * Construct with pointer to \p source widget and \p shortcutMap
     * @param source Pointer to source widget
     * @param shortcutMap Const reference to the shortcut map for which to create the overlay
     */
    ShortcutMapOverlayWidget(QWidget* source, const util::ShortcutMap& shortcutMap);

    /**
     * Get widget fader
     * @return Reference to widget fader
     */
    util::WidgetFader& getWidgetFader();

private:
    const util::ShortcutMap&    _shortcutMap;       /** Const reference to the shortcut map for which to create the overlay */
    mv::util::WidgetOverlayer   _widgetOverlayer;   /** Utility for layering on top of the target widget */
    mv::util::WidgetFader       _widgetFader;       /** Widget fader for animating the widget opacity */
};

}
