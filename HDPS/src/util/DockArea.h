#pragma once

#include <QMap>
#include <QString>
#include <QIcon>

namespace hdps::gui
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
    { "Top", static_cast<DockAreaFlag>(DockAreaFlag::Top) },
    { "Bottom", static_cast<DockAreaFlag>(DockAreaFlag::Bottom) },
    { "Center", static_cast<DockAreaFlag>(DockAreaFlag::Center) }
});

QIcon getDockAreaIcon(DockAreaFlag dockArea);

}