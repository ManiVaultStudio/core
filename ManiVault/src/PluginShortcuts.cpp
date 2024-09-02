// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginShortcuts.h"

#include "Plugin.h"

using namespace mv::util;
using namespace mv::plugin;

namespace mv::gui {

PluginShortcuts::PluginShortcuts(plugin::Plugin* plugin) :
    QObject(plugin),
    _plugin(plugin),
    _viewShortcutMapAction(this, "Shortcut map")
{
}

util::ShortcutMap& PluginShortcuts::getShortcutMap()
{
    return const_cast<PluginFactory*>(_plugin->getFactory())->getShortcutMap();
}

const util::ShortcutMap& PluginShortcuts::getShortcutMap() const
{
    return const_cast<PluginShortcuts*>(this)->getShortcutMap();
}

void PluginShortcuts::addShortcut(const util::ShortcutMap::Shortcut& shortcut)
{
    getShortcutMap().addShortcut(shortcut);
}

void PluginShortcuts::removeShortcut(const util::ShortcutMap::Shortcut& shortcut)
{
    getShortcutMap().removeShortcut(shortcut);
}

util::ShortcutMap::Shortcuts PluginShortcuts::getShortcuts(const QStringList& categories) const
{
    return getShortcutMap().getShortcuts(categories);
}

bool PluginShortcuts::hasShortcuts(const QStringList& categories) const
{
    return getShortcutMap().hasShortcuts(categories);
}

void PluginShortcuts::viewShortcutMap()
{
    qDebug() << __FUNCTION__ << "not yet implemented...";
}

}
