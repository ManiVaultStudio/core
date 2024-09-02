// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/VerticalGroupAction.h"

namespace mv::gui {

/**
 * Note: This action is developed for internal use only
 */
class CORE_EXPORT PluginLearningCenterAction : public VerticalGroupAction
{
public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    PluginLearningCenterAction(QObject* parent = nullptr);

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

public: // Help

    /**
     * Get whether the plugin has help information or not
     * @return Boolean determining whether the plugin has help information or not
     */
    virtual bool hasHelp();

    /**
     * Get trigger action that shows help in some form (will be added to help menu, and if it is a view plugin also to the tab toolbar)
     * @return Reference to show help trigger action (maybe nullptr if the plugin does not provide any help)
     */
    virtual gui::TriggerAction& getTriggerHelpAction() final;

public: // Properties

    /**
     * Get property in variant form
     * @param name Name of the property
     * @param defaultValue Default value
     * @return Property in variant form
     */
    QVariant getProperty(const QString& name, const QVariant& defaultValue = QVariant()) const;

    /**
    * Set property
    * @param name Name of the property
    * @param value Property value
    */
    void setProperty(const QString& name, const QVariant& value);

    /**
    * Determines whether a property with a give name exists
    * @param name Name of the property
    * @param value If property with the given name exists
    */
    bool hasProperty(const QString& name) const;

    /** Returns a list of available property names */
    QStringList propertyNames() const;

public: // Settings

    /**
     * Load setting
     * @param path Path of the setting (e.g. General/Computation/NumberOfIterations)
     * @param defaultValue Value to return if the setting with \p path was not found
     * @return Plugin setting in variant form
     */
    QVariant getSetting(const QString& path, const QVariant& defaultValue = QVariant()) const;

    /**
     * Save setting
     * @param path Path of the setting (e.g. General/Computation/NumberOfIterations)
     * @param value Value in variant form
     */
    void setSetting(const QString& path, const QVariant& value);

    /**
     * Get global settings prefix (facade for the PluginFactory class)
     * @return Plugin global settings prefix
     */
    QString getGlobalSettingsPrefix() const;

    /**
     * Get global settings action (facade for the PluginFactory class)
     * @return Pointer to plugin global settings action (maybe nullptr if plugin does not have global settings)
     */
    gui::PluginGlobalSettingsGroupAction* getGlobalSettingsAction() const;

public: // Views

    /** View plugin description */
    virtual void viewDescription() { qDebug() << __FUNCTION__ << "not yet implemented..."; };

    /** View plugin shortcut map */
    virtual void viewShortcutMap() { qDebug() << __FUNCTION__ << "not yet implemented..."; };

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

public: // Miscellaneous

    /** Destroys the plugin and removes it from the plugin manager */
    virtual void destroy() final;

public: // Video tags

    /**
     * Get video tags
     * @return Video tags
     */
    const QStringListModel& getVideoTags() const;

protected: // Video tags

    /**
     * Get video tags
     * @return Video tags
     */
    QStringListModel& getVideoTags();

public: // Action getters

    //TriggerAction& getViewDescriptionAction() { return _viewDescriptionAction; };
    TriggerAction& getViewShortcutMapAction() { return _viewShortcutMapAction; };

    //const TriggerAction& getViewDescriptionAction() const { return _viewDescriptionAction; };
    const TriggerAction& getViewShortcutMapAction() const { return _viewShortcutMapAction; };

protected:
    //TriggerAction          _viewDescriptionAction;     /** Trigger action that displays the plugin description */
    TriggerAction          _viewShortcutMapAction;     /** Trigger action that displays the plugin shortcut map */
    QStringListModel            _videoTags;                 /** For linking to the learning center videos */

    friend class PluginFactory;
};

}
