#ifndef HDPS_PLUGIN_WRITER_PLUGIN_H
#define HDPS_PLUGIN_WRITER_PLUGIN_H

#include "PluginFactory.h"

#include <QString>

namespace hdps {

namespace plugin {

class WriterPlugin : public Plugin
{
public:
    WriterPlugin(QString name) : Plugin(Type::WRITER, name) { }

    virtual void writeDataToFile(QString filename) = 0;

    virtual ~WriterPlugin() {};
};


class WriterPluginFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    
    virtual ~WriterPluginFactory() {};
    
    virtual WriterPlugin* produce() = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::WriterPluginFactory, "cytosplore.WriterPluginFactory")

#endif // HDPS_PLUGIN_WRITER_PLUGIN_H
