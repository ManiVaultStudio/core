#ifndef HDPS_PLUGIN_PLUGIN_H
#define HDPS_PLUGIN_PLUGIN_H

#include "CoreInterface.h"
#include "PluginType.h"

#include <QString>

namespace hdps {

namespace plugin {

class Plugin
{
public:
    Plugin(Type type, QString name) : _type(type), _name(name) { }
    virtual ~Plugin() {};

    /**
     * Can be implemented to initialize the plugin to a certain state.
     * This function gets called when the plugin is first instantiated.
     */
    virtual void init() = 0;
	
    /** Returns the type of plugin, see all types in PluginType */
    Type getType() const { return _type; }

    /**
     * Returns the kind of plugin. The kind is specific to the
     * particular implementation of a plugin type.
     */
    QString getName() const { return _name; }

    /**
     * Returns the version of the plugin. If no version string is implemented 
     * by this particular plugin it will return the "No version" string.
     */
    virtual QString getVersion() const { return "No Version"; }

    /**
     * Stores a reference to the core in the plugin via the CoreInterface
     * class, which provides restricted access to the core.
     */
    void setCore(CoreInterface* core) {
        this->_core = core;
    }
protected:
    CoreInterface* _core;

private:
    const Type _type;
    const QString _name;
};

} // namespace plugin

} // namespace hdps

#endif // HDPS_PLUGIN_PLUGIN_H
