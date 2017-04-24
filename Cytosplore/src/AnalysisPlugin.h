#ifndef HDPS_PLUGIN_ANALYSIS_PLUGIN
#define HDPS_PLUGIN_ANALYSIS_PLUGIN

#include "PluginFactory.h"

namespace hdps {

namespace plugin {

class AnalysisPlugin : public Plugin
{
public:
    AnalysisPlugin(QString name) : Plugin(Type::ANALYSIS, name) { }
    virtual ~AnalysisPlugin() {};
};


class AnalysisPluginFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    
    virtual ~AnalysisPluginFactory() {};
    
    virtual AnalysisPlugin* produce() = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::AnalysisPluginFactory, "cytosplore.AnalysisPluginFactory")

#endif // HDPS_PLUGIN_ANALYSIS_PLUGIN
