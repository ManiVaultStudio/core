#ifndef HDPS_PLUGIN_H
#define HDPS_PLUGIN_H

#include "PluginFactory.h"
#include "CoreInterface.h"
#include "PluginType.h"
#include "Application.h"
#include "event/EventListener.h"
#include "actions/WidgetAction.h"
#include "actions/StringAction.h"

#include <QString>
#include <QMap>
#include <QVariant>
#include <QUuid>
#include <QVariant>
#include <QIcon>

class QMenu;

namespace hdps
{
namespace plugin
{

class Plugin : public hdps::gui::WidgetAction
{
public:
    Plugin(const PluginFactory* factory);

    virtual ~Plugin() {};

    /**
     * Can be implemented to initialize the plugin to a certain state.
     * This function gets called when the plugin is first instantiated.
     */
    virtual void init() = 0;

    /** Returns the unique name of this plugin */
    QString getName() const {
        return _name;
    }

    /** Returns the GUI name of this plugin */
    QString getGuiName() const {
        return _guiNameAction.getString();
    }

    /** Returns the icon of this plugin */
    virtual QIcon getIcon() const final {
        return _factory->getIcon();
    }

    /**
    * Returns the kind of plugin. The kind is specific to the
    * particular implementation of a plugin type.
    */
    QString getKind() const
    {
        return _factory->getKind();
    }

    /**
     * Returns the type of plugin, see all types in PluginType
     */
    Type getType() const
    {
        return _factory->getType();
    }

    /**
     * Returns the version of the plugin. If no version string is implemented 
     * by this particular plugin it will return the "No version" string.
     */
    virtual QString getVersion() const
    {
        return "No Version";
    }

    /**
     * Stores a reference to the core in the plugin via the CoreInterface
     * class, which provides restricted access to the core.
     */
    void setCore(CoreInterface* core)
    {
        this->_core = core;

        _eventListener.setEventCore(core);
    }

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
    QVariant getProperty(const QString& name, const QVariant& defaultValue = QVariant()) const
    {
        if (!hasProperty(name))
            return defaultValue;

        return _properties[name];
    }

    /**
    * Set property
    * @param name Name of the property
    * @param value Property value
    */
    void setProperty(const QString& name, const QVariant& value)
    {
        _properties[name] = value;
    }

    /**
    * Determines whether a property with a give name exists
    * @param name Name of the property
    * @param value If property with the given name exists
    */
    bool hasProperty(const QString& name) const
    {
        return _properties.contains(name);
    }

    /** Returns a list of available property names */
    QStringList propertyNames() const
    {
        return _properties.keys();
    }

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

    /**
     * Get number of plugin instances
     * @param pluginKind The kind of plugin
     * @return The number of plugin instances
     */
    static std::uint32_t getNumberOfInstances(const QString& pluginKind)
    {
        return Plugin::noInstances[pluginKind];
    }

public: // Action getters

    gui::StringAction& getGuiNameAction() { return _guiNameAction; }

protected:
    CoreInterface* _core;

protected:
    const PluginFactory*        _factory;           /** Pointer to plugin factory */
    const QString               _name;              /** Unique plugin name */
    QMap<QString, QVariant>     _properties;        /** Properties map */
    EventListener               _eventListener;     /** Listen to public events */
    gui::StringAction           _guiNameAction;     /** Action for the GUI name */

    /** Keeps track of how many instance have been created per plugin kind */
    static QMap<QString, std::int32_t> noInstances;

    friend class PluginFactory;
};

} // namespace plugin

} // namespace hdps

#endif // HDPS_PLUGIN_H
