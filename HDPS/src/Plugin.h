#ifndef HDPS_PLUGIN_H
#define HDPS_PLUGIN_H

#include "CoreInterface.h"
#include "PluginType.h"
#include "Application.h"
#include "event/EventListener.h"
#include "actions/WidgetAction.h"

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

class Plugin : public hdps::EventListener
{
public:
    using QActionList = QList<gui::WidgetAction*>;

public:
    Plugin(Type type, QString kind);

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
        return _guiName;
    }

    /** Returns the icon of this plugin */
    virtual QIcon getIcon() const {
        return Application::getIconFont("FontAwesome").getIcon("plug");
    }

    /**
    * Returns the kind of plugin. The kind is specific to the
    * particular implementation of a plugin type.
    */
    QString getKind() const
    {
        return _kind;
    }

    /**
     * Returns the type of plugin, see all types in PluginType
     */
    Type getType() const
    {
        return _type;
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

        setEventCore(core);
    }

    /**
     * Returns a list of datatypes that is supported for operations by this plugin.
     */
    virtual DataTypes supportedDataTypes() const = 0;

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

public: // GUI

    /**
     * Generates a context menu for display in other (view) plugins
     * @param context Context of the plugin that requested the context menu
     * @return Context menu
     */
    virtual QMenu* contextMenu(const QVariant& context) { return nullptr; };

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

public: // Actions API

    /**
     * Add widget action to the plugin (expose it)
     * The plugin does not take ownership of the allocated widget action
     * @param widgetAction Widget action to add
     */
    void addActionWidget(gui::WidgetAction* widgetAction);

    /**
     * Removes an action widget from the plugin
     * This does not de-allocate the widget action memory
     * @param widgetAction Widget action to remove
     */
    void removeActionWidget(gui::WidgetAction* widgetAction);

    /** Returns the list of exposed action widgets */
    const QActionList& getActionWidgets() const;

protected:
    CoreInterface* _core;

private:
    const QString               _name;              /** Unique plugin name */
    const QString               _guiName;           /** Name in the GUI */
    const QString               _kind;              /** Kind of plugin (e.g. scatter plot plugin & TSNE analysis plugin) */
    const Type                  _type;              /** Type of plugin (e.g. analysis, data, loader, writer & view) */
    QMap<QString, QVariant>     _properties;        /** Properties map */
    QActionList                 _widgetActions;     /** Exposed widget actions */

    /** Keeps track of how many instance have been created per plugin kind */
    static QMap<QString, std::int32_t> _noInstances;
};

} // namespace plugin

} // namespace hdps

#endif // HDPS_PLUGIN_H
