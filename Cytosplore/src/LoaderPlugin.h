#ifndef HDPS_PLUGIN_LOADER_PLUGIN
#define HDPS_PLUGIN_LOADER_PLUGIN

#include "PluginFactory.h"

namespace hdps {

namespace plugin {

class LoaderPlugin : public Plugin
{
public:
    LoaderPlugin(QString name) : Plugin(Type::LOADER, name) { }
    virtual ~LoaderPlugin() {};
};


class LoaderPluginFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    
    virtual ~LoaderPluginFactory() {};
    
    virtual LoaderPlugin* produce() = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::LoaderPluginFactory, "cytosplore.LoaderPluginFactory")

#endif // HDPS_PLUGIN_LOADER_PLUGIN
