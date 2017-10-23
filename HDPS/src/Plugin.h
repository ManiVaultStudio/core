#ifndef HDPS_PLUGIN_PLUGIN_H
#define HDPS_PLUGIN_PLUGIN_H

#include "CoreInterface.h"
#include "PluginType.h"

#include <QString>
#include <QUuid>

namespace hdps
{
namespace plugin
{

class Plugin
{
public:
    Plugin(Type type, QString kind) : _type(type), _kind(kind), _name(kind + QUuid::createUuid().toString()) { }
    virtual ~Plugin() {};

    /**
     * Can be implemented to initialize the plugin to a certain state.
     * This function gets called when the plugin is first instantiated.
     */
    virtual void init() = 0;
	
    /**
     * Returns the unique name of this plugin.
     */
    QString getName() const
    {
        return _name;
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
protected:
    CoreInterface* _core;

private:
    const QString _name;
    const QString _kind;
    const Type _type;
};

} // namespace plugin

} // namespace hdps

#endif // HDPS_PLUGIN_PLUGIN_H
