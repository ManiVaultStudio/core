// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "PluginFactory.h"
#include "PluginType.h"

#include "event/EventListener.h"

#include "actions/WidgetAction.h"
#include "actions/StringAction.h"
#include "actions/TriggerAction.h"

#include <QString>
#include <QMap>
#include <QVariant>
#include <QIcon>

class QMenu;

namespace mv::plugin
{

class Plugin : public mv::gui::WidgetAction
{
public:

    /**
     * Construct with pointer to plugin \p factory
     * @param factory Pointer to plugin factory
     */
    Plugin(const PluginFactory* factory);

    virtual ~Plugin();

    /**
     * Can be implemented to initialize the plugin to a certain state.
     * This function gets called when the plugin is first instantiated.
     */
    virtual void init() {};

    /**
     * Get the plugin factory for the plugin
     * @return Pointer to plugin factory
     */
    virtual const PluginFactory* getFactory() const final;

    /** Returns the unique name of this plugin */
    QString getName() const;

    /** Returns the GUI name of this plugin */
    QString getGuiName() const;

    /** Returns the icon of this plugin */
    virtual QIcon getIcon() const final;

    /**
    * Returns the kind of plugin. The kind is specific to the
    * particular implementation of a plugin type.
    */
    QString getKind() const;

    /**
     * Returns the type of plugin, see all types in PluginType
     */
    Type getType() const;

    /**
     * Returns the version of the plugin. If no version string is implemented 
     * by this particular plugin it will return the "No version" string.
     */
    virtual QString getVersion() const;

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

public: // Action getters

    gui::StringAction& getGuiNameAction() { return _guiNameAction; };
    gui::TriggerAction& getDestroyAction() { return _destroyAction; }

protected:
    CoreInterface*              _core;              /** Pointer to the core interface */
    const PluginFactory*        _factory;           /** Pointer to plugin factory */
    const QString               _name;              /** Unique plugin name */
    QMap<QString, QVariant>     _properties;        /** Properties map */
    EventListener               _eventListener;     /** Listen to public events */
    gui::StringAction           _guiNameAction;     /** Action for the GUI name */
    gui::TriggerAction          _destroyAction;     /** Action for destroying the plugin */

    friend class PluginFactory;
};

}
