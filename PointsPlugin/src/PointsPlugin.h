#ifndef POINTS_PLUGIN_H
#define POINTS_PLUGIN_H

#include <DataTypePlugin.h>

#include <vector>

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

class PointsPlugin : public DataTypePlugin
{
public:
    PointsPlugin() : DataTypePlugin("Points") { }
    ~PointsPlugin(void);
    
    void init();

    std::vector<float> data;
};


// =============================================================================
// Factory
// =============================================================================

class PointsPluginFactory : public DataTypePluginFactory
{
    Q_INTERFACES(hdps::plugin::DataTypePluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.PointsPlugin"
                      FILE  "PointsPlugin.json")
    
public:
    PointsPluginFactory(void) {}
    ~PointsPluginFactory(void) {}
    
    DataTypePlugin* produce();
};

#endif // POINTS_PLUGIN_H
