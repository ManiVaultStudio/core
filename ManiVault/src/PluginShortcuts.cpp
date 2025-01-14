// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginShortcuts.h"

#include "Plugin.h"

using namespace mv::gui;
using namespace mv::plugin;

namespace mv {

PluginShortcuts::PluginShortcuts(plugin::Plugin* plugin) :
    QObject(plugin),
    _plugin(plugin),
    _viewAction(this, "Shortcut map")
{
    _viewAction.setToolTip("View shortcuts");
    _viewAction.setIconByName("keyboard");
    _viewAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::HiddenInActionContextMenu);
    _viewAction.setConnectionPermissionsToForceNone();

    connect(&_viewAction, &TriggerAction::triggered, this, &PluginShortcuts::view);
}

util::ShortcutMap& PluginShortcuts::getMap()
{
    return const_cast<PluginFactory*>(_plugin->getFactory())->getPluginMetaData().getShortcutMap();
}

const util::ShortcutMap& PluginShortcuts::getMap() const
{
    return const_cast<PluginShortcuts*>(this)->getMap();
}

void PluginShortcuts::add(const util::ShortcutMap::Shortcut& shortcut)
{
    getMap().addShortcut(shortcut);
}

void PluginShortcuts::remove(const util::ShortcutMap::Shortcut& shortcut)
{
    getMap().removeShortcut(shortcut);
}

util::ShortcutMap::Shortcuts PluginShortcuts::getShortcuts(const QStringList& categories) const
{
    return getMap().getShortcuts(categories);
}

bool PluginShortcuts::hasShortcuts(const QStringList& categories) const
{
    return getMap().hasShortcuts(categories);
}

void PluginShortcuts::view()
{
    qDebug() << __FUNCTION__ << "not yet implemented...";
}

}
