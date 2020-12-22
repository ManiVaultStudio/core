#ifndef HDPS_PLUGIN_H
#define HDPS_PLUGIN_H

#include "CoreInterface.h"
#include "PluginType.h"

#include <QString>
#include <QUuid>
#include <QVariant>

namespace hdps
{
namespace plugin
{

class Plugin
{
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

protected:
    CoreInterface* _core;

private:
    const QString               _name;          /** Unique plugin name */
    const QString               _guiName;       /** Name in the GUI */
    const QString               _kind;          /** Kind of plugin (e.g. scatter plot plugin & TSNE analysis plugin) */
    const Type                  _type;          /** Type of plugin (e.g. analysis, data, loader, writer & view) */
    QMap<QString, QVariant>     _properties;    /** Properties map */

    /** Keeps track of how many instance have been created per plugin kind */
    static QMap<QString, std::int32_t> _noInstances;
};

} // namespace plugin

} // namespace hdps

#endif // HDPS_PLUGIN_H