#pragma once

#include "ManiVaultGlobals.h"

#include <QMenu>
#include <QString>

#include <functional>

namespace mv::developer
{
using MenuPopulator = std::function<void(QMenu&, QWidget*)>;

CORE_EXPORT void registerMenuPopulator(const QString& id, MenuPopulator populator);
CORE_EXPORT void populateMenu(QMenu& menu, QWidget* owner);
}
