#ifndef HDPS_ANALYSISPLUGIN_H
#define HDPS_ANALYSISPLUGIN_H

#include "PluginFactory.h"
#include "DataConsumer.h"

#include <memory>

class QWidget;

namespace hdps
{
namespace plugin
{

class AnalysisPlugin : public Plugin, public DataConsumer
{
public:
    AnalysisPlugin(QString name) : Plugin(Type::ANALYSIS, name) { }
    ~AnalysisPlugin() override {};

    bool hasSettings()
    {
        return getSettings() != nullptr;
    }

    virtual QWidget* const getSettings() = 0;
};


class AnalysisPluginFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    
    ~AnalysisPluginFactory() override {};
    
    /**
    * Produces an instance of an analysis plugin. This function gets called by the plugin manager.
    */
    AnalysisPlugin* produce() override = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::AnalysisPluginFactory, "cytosplore.AnalysisPluginFactory")

#endif // HDPS_ANALYSISPLUGIN_H
