#ifndef POINTS_2D_PLUGIN_H
#define POINTS_2D_PLUGIN_H

#include <DataTypePlugin.h>

#include <vector>

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

class Points2DPlugin : public DataTypePlugin
{
public:
    Points2DPlugin() : DataTypePlugin("2D Points") { }
    ~Points2DPlugin(void);
    
    void init();

    std::vector<float> data;
};


// =============================================================================
// Factory
// =============================================================================

class Points2DPluginFactory : public DataTypePluginFactory
{
    Q_INTERFACES(hdps::plugin::DataTypePluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.Points2DPlugin"
                      FILE  "Points2DPlugin.json")
    
public:
    Points2DPluginFactory(void) {}
    ~Points2DPluginFactory(void) {}
    
    DataTypePlugin* produce();
};

#endif // POINTS_2D_PLUGIN_H
