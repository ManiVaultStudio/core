#ifndef HDPS_PLUGIN_ANALYSIS_PLUGIN
#define HDPS_PLUGIN_ANALYSIS_PLUGIN

#include "PluginFactory.h"
#include "DataConsumer.h"
#include "widgets/SettingsWidget.h"

#include <memory>

namespace hdps {

namespace plugin {

class AnalysisPlugin : public Plugin, public DataConsumer
{
public:
    AnalysisPlugin(QString name) : Plugin(Type::ANALYSIS, name) { }
    virtual ~AnalysisPlugin() {};

protected:
    std::unique_ptr<SettingsWidget> _settings;
};


class AnalysisPluginFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    
    virtual ~AnalysisPluginFactory() {};
    
    /**
    * Produces an instance of an analysis plugin. This function gets called by the plugin manager.
    */
    virtual AnalysisPlugin* produce() = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::AnalysisPluginFactory, "cytosplore.AnalysisPluginFactory")

#endif // HDPS_PLUGIN_ANALYSIS_PLUGIN
