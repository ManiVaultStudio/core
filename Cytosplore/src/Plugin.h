#ifndef HDPS_PLUGIN_PLUGIN_H
#define HDPS_PLUGIN_PLUGIN_H

#include "PluginType.h"

#include <QString>

namespace hdps {

namespace plugin {

class Plugin
{
public:
    Plugin(Type type, QString name) : _type(type), _name(name) { }
    virtual ~Plugin() {};

    virtual void init() = 0;
	
    Type getType() const { return _type; }
    QString getName() const { return _name; }
    virtual QString getVersion() const { return "No Version"; }
private:
    const Type _type;
    const QString _name;
};

} // namespace plugin

} // namespace hdps

#endif // HDPS_PLUGIN_PLUGIN_H
