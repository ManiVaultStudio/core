#pragma once

#include "ManiVaultGlobals.h"

#include <QMenu>
#include <QString>

#include <functional>

namespace mv::developer
{
/** Callback that adds developer-only actions to a submenu owned by the caller. */
using MenuPopulator = std::function<void(QMenu&, QWidget*)>;

/** Registers a developer-menu populator under a stable identifier. Empty IDs or callbacks are ignored. */
CORE_EXPORT void registerMenuPopulator(const QString& id, MenuPopulator populator);

/** Invokes a snapshot of all registered developer-menu populators for the supplied menu. */
CORE_EXPORT void populateMenu(QMenu& menu, QWidget* owner);
}
