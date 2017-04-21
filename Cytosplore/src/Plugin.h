#ifndef HDPS_PLUGIN_PLUGIN_H
#define HDPS_PLUGIN_PLUGIN_H

#include <QString>

namespace hdps {

namespace plugin {

class Plugin
{
public:
    Plugin(QString name) : _name(name) { }
    virtual ~Plugin() {};

    virtual void init() = 0;

    QString getName() { return _name; }
    virtual QString getVersion() { return "No Version"; }
private:
    QString _name;
};

} // namespace plugin

} // namespace hdps

#endif // HDPS_PLUGIN_PLUGIN_H
