#ifndef SELECTION_PLUGIN_H
#define SELECTION_PLUGIN_H

#include <DataTypePlugin.h>

#include <QString>
#include <vector>

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

class SelectionPlugin : public DataTypePlugin
{
public:
    SelectionPlugin() : DataTypePlugin("Selection") { }
    ~SelectionPlugin(void);
    
    void init();

    QString parentName;
    std::vector<unsigned int> indices;
};


// =============================================================================
// Factory
// =============================================================================

class SelectionPluginFactory : public DataTypePluginFactory
{
    Q_INTERFACES(hdps::plugin::DataTypePluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.SelectionPlugin"
                      FILE  "SelectionPlugin.json")
    
public:
    SelectionPluginFactory(void) {}
    ~SelectionPluginFactory(void) {}
    
    DataTypePlugin* produce();
};

#endif // SELECTION_PLUGIN_H
