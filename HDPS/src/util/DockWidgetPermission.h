#pragma once

#include <QMap>
#include <QString>

namespace hdps::util
{

/** Dock widget permission */
enum class DockWidgetPermission {
    MayClose    = 0x00,     /** Dock widget may be closed */
    MayMove     = 0x01,     /** Dock widget may be moved */
    MayFloat    = 0x02,     /** Dock widget may be floated */
    
    All = MayClose | MayMove | MayFloat     /** All docking permissions */
};

Q_DECLARE_FLAGS(DockWidgetPermissions, DockWidgetPermission)

/** Maps dock widget permission flag to string */
static QMap<DockWidgetPermission, QString> dockWidgetPermissionNames({
    { DockWidgetPermission::MayClose, "May Close" },
    { DockWidgetPermission::MayMove, "May Move" },
    { DockWidgetPermission::MayFloat, "May Float" }
});

}
