// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/TriggerAction.h"

#include <QObject>

namespace mv::plugin {
    class Plugin;
}

namespace mv {

/**
 * Plugin shortcuts class
 *
 * This class is a facade of mv::util::ShortcutMap
 *
 */
class CORE_EXPORT PluginShortcuts final : public QObject
{
public:

    /**
     * Construct with pointer to \p plugin and \p parent object
     * @param plugin Pointer to plugin
     */
    PluginShortcuts(plugin::Plugin* plugin);

public: // Shortcuts

    /**
     * Get the shortcut map
     * @return Shortcut map
     */
    util::ShortcutMap& getMap();

    /**
     * Get the shortcut map
     * @return Shortcut map
     */
    const util::ShortcutMap& getMap() const;

    /**
     * Add \p shortcut to the map
     * @param shortcut Shortcut to add
     */
    void add(const util::ShortcutMap::Shortcut& shortcut);

    /**
     * Remove \p shortcut from the map
     * @param shortcut Shortcut to remove
     */
    void remove(const util::ShortcutMap::Shortcut& shortcut);

    /**
     * Get shortcuts for \p categories
     * @param categories Categories to retrieve shortcuts for (all shortcuts if empty)
     * @return Vector of shortcuts
     */
    util::ShortcutMap::Shortcuts getShortcuts(const QStringList& categories = QStringList()) const;

    /**
     * Establish whether any shortcut exists for \p categories
     * @param categories Categories to check (all categories if empty)
     * @return Boolean determining whether any shortcut exists
     */
    bool hasShortcuts(const QStringList& categories = QStringList()) const;

    /** View shortcut map */
    void view();

public: // Action getters

    //TriggerAction& getViewDescriptionAction() { return _viewDescriptionAction; };
    gui::TriggerAction& getViewShortcutMapAction() { return _viewAction; };

    //const TriggerAction& getViewDescriptionAction() const { return _viewDescriptionAction; };
    const gui::TriggerAction& getViewShortcutMapAction() const { return _viewAction; };

protected:
    const plugin::Plugin*   _plugin;        /** Pointer to parent plugin */
    gui::TriggerAction      _viewAction;    /** Trigger action that displays the plugin shortcuts */
};

}
