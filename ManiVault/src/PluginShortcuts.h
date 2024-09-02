// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QObject>

namespace mv::plugin {
    class Plugin;
}

namespace mv::gui {

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
     * @param parent Pointer to parent object
     */
    PluginShortcuts(plugin::Plugin* plugin);

public: // Shortcuts

    /**
     * Get the shortcut map
     * @return Shortcut map
     */
    util::ShortcutMap& getShortcutMap();

    /**
     * Get the shortcut map
     * @return Shortcut map
     */
    const util::ShortcutMap& getShortcutMap() const;

    /**
     * Add \p shortcut to the map
     * @param shortcut Shortcut to add
     */
    void addShortcut(const util::ShortcutMap::Shortcut& shortcut);

    /**
     * Remove \p shortcut from the map
     * @param shortcut Shortcut to remove
     */
    void removeShortcut(const util::ShortcutMap::Shortcut& shortcut);

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
    void viewShortcutMap();

public: // Action getters

    //TriggerAction& getViewDescriptionAction() { return _viewDescriptionAction; };
    TriggerAction& getViewShortcutMapAction() { return _viewShortcutMapAction; };

    //const TriggerAction& getViewDescriptionAction() const { return _viewDescriptionAction; };
    const TriggerAction& getViewShortcutMapAction() const { return _viewShortcutMapAction; };

protected:
    const plugin::Plugin*   _plugin;                    /** Pointer to parent plugin */
    TriggerAction           _viewShortcutMapAction;     /** Trigger action that displays the plugin shortcuts */
};

}
