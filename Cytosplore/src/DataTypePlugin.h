#ifndef HDPS_PLUGIN_DATA_TYPE_PLUGIN
#define HDPS_PLUGIN_DATA_TYPE_PLUGIN

#include "PluginFactory.h"

namespace hdps {

namespace plugin {

class DataTypePlugin : public Plugin
{
public:
    DataTypePlugin(QString name) : Plugin(Type::DATA_TYPE, name) { }
    virtual ~DataTypePlugin() {};
};


class DataTypePluginFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    
    virtual ~DataTypePluginFactory() {};
    
    virtual DataTypePlugin* produce() = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::DataTypePluginFactory, "cytosplore.DataTypePluginFactory")

#endif // HDPS_PLUGIN_DATA_TYPE_PLUGIN
