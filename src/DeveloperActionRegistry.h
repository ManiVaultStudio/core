#pragma once

#include "ManiVaultGlobals.h"

#include <QMenu>
#include <QString>

#include <functional>

namespace mv::developer
{
/** Callback that populates a developer submenu. */
using MenuPopulator = std::function<void(QMenu&, QWidget*)>;

/** Registers a developer-menu populator under a stable identifier. */
CORE_EXPORT void registerMenuPopulator(const QString& id, MenuPopulator populator);

/** Invokes all registered developer-menu populators for the supplied menu. */
CORE_EXPORT void populateMenu(QMenu& menu, QWidget* owner);
}
