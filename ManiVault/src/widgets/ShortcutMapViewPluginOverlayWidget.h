// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "widgets/ViewPluginOverlayWidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>

namespace mv::util {
    class ShortcutMap;
}

namespace mv::gui
{

/**
 * Shortcut map view plugin overlay widget class
 *
 * Overlays the view plugin source widget with a shortcut map cheat sheet
 *  
 * @author Thomas Kroes
 */
class ShortcutMapViewPluginOverlayWidget : public ViewPluginOverlayWidget
{
public:

    /**
     * Construct with pointer to source \p view plugin
     * @param viewPlugin Pointer to source view plugin
     */
    ShortcutMapViewPluginOverlayWidget(plugin::ViewPlugin* viewPlugin);

private:
    const util::ShortcutMap&    _shortcutMap;               /** Const reference to the shortcut map for which to create the overlay */
    QHBoxLayout                 _headerLayout;              /** Layout for the header */
    QLabel                      _headerIconLabel;           /** Label for header icon */
    QLabel                      _headerTextLabel;           /** Label for header text */
    QScrollArea                 _shortcutsScrollArea;       /** Scroll area for the shortcut */
    QWidget                     _shortcutsWidget;           /** Widget with the shortcuts label */
    QVBoxLayout                 _shortcutsWidgetLayout;     /** Widget with the shortcuts label */
    QLabel                      _bodyLabel;                 /** Shortcut cheatsheet HTML */
};

}
