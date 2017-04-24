#ifndef HDPS_PLUGIN_TRANSFORMATION_PLUGIN
#define HDPS_PLUGIN_TRANSFORMATION_PLUGIN

#include "PluginFactory.h"

namespace hdps {

namespace plugin {

class TransformationPlugin : public Plugin
{
public:
    TransformationPlugin(QString name) : Plugin(Type::TRANFORMATION, name) { }
    virtual ~TransformationPlugin() {};
};


class TransformationPluginFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    
    virtual ~TransformationPluginFactory() {};
    
    virtual TransformationPlugin* produce() = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::TransformationPluginFactory, "cytosplore.TransformationPluginFactory")

#endif // HDPS_PLUGIN_TRANSFORMATION_PLUGIN
