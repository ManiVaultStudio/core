// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QMap>
#include <QString>
#include <QIcon>

namespace mv::gui
{

/** Dock area flags */
enum class DockAreaFlag {
    None    = 0x00,     /** No docking area */
    Left    = 0x01,     /** Left docking area */
    Right   = 0x02,     /** Right docking area */
    Top     = 0x04,     /** Top docking area */
    Bottom  = 0x08,     /** Bottom docking area */
    Center  = 0x10,     /** Center docking area */

    InvalidDockWidgetArea   = None,
    OuterDockAreas          = Top | Left | Right | Bottom,
    AllDockAreas            = OuterDockAreas | Center
};

Q_DECLARE_FLAGS(DockAreaFlags, DockAreaFlag)

/** Maps dock area string to dock area flag */
static QMap<QString, DockAreaFlag> dockAreaMap({
    { "None", static_cast<DockAreaFlag>(DockAreaFlag::None) },
    { "Left", static_cast<DockAreaFlag>(DockAreaFlag::Left) },
    { "Right", static_cast<DockAreaFlag>(DockAreaFlag::Right) },
    { "Above", static_cast<DockAreaFlag>(DockAreaFlag::Top) },
    { "Beneath", static_cast<DockAreaFlag>(DockAreaFlag::Bottom) },
    { "Tabbed", static_cast<DockAreaFlag>(DockAreaFlag::Center) }
});

QIcon getDockAreaIcon(DockAreaFlag dockArea);

/** Auto-hide location flags */
enum class AutoHideLocation {
    Top,
    Left,
    Right,
    Bottom
};

}
