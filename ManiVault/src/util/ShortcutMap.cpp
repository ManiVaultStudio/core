// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ShortcutMap.h"

#ifdef _DEBUG
    #define SHORTCUT_MAP_VERBOSE
#endif

namespace mv::util {

ShortcutMap::ShortcutMap(const QString& title, QObject* parent) :
    QObject(parent),
    _title(title)
{
}

void ShortcutMap::setTitle(const QString& title)
{
    if (title == _title)
        return;

    _title = title;

    emit titleChanged(_title);
}

std::shared_ptr<gui::ShortcutMapOverlayWidget> ShortcutMap::createShortcutMapOverlayWidget(QWidget* source) const
{
    return std::make_shared<gui::ShortcutMapOverlayWidget>(source, *this);
}

void ShortcutMap::addShortcut(const Shortcut& shortcut)
{
    _shortcuts.push_back(shortcut);

    emit shortcutAdded(shortcut);
}

void ShortcutMap::removeShortcut(const Shortcut& shortcut)
{
    emit shortcutAboutToBeRemoved(shortcut);
    {
        _shortcuts.erase(std::remove(_shortcuts.begin(), _shortcuts.end(), shortcut), _shortcuts.end());
    }
    emit shortcutRemoved(shortcut);
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
