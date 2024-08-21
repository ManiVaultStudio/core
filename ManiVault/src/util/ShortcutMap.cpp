// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ShortcutMap.h"

#ifdef _DEBUG
    #define SHORTCUT_MAP_VERBOSE
#endif

namespace mv::util {

ShortcutMap::ShortcutMap(QObject* parent) :
    QObject(parent)
{
}

std::shared_ptr<gui::ShortcutMapOverlayWidget> ShortcutMap::createShortcutMapOverlayWidget(QWidget* source) const
{
    return std::make_shared<gui::ShortcutMapOverlayWidget>(source, *this);
}

ShortcutMap::Shortcuts ShortcutMap::getShortcuts(const QStringList& categories /*= QStringList()*/) const
{
    if (categories.isEmpty())
        return _shortcuts;

    Shortcuts shortcuts;

    for (const auto& shortcut : _shortcuts)
        if (categories.contains(shortcut._category))
            shortcuts.push_back(shortcut);

    return shortcuts;
}

bool ShortcutMap::hasShortcuts(const QStringList& categories /*= QStringList()*/) const
{
    return !getShortcuts(categories).empty();
}

}
