#ifndef POINTS_PLUGIN_H
#define POINTS_PLUGIN_H

#include <DataTypePlugin.h>

#include "Set.h"
#include <QString>
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

    unsigned int getNumPoints() const
    {
        return data.size() / numDimensions;
    }

    unsigned int getNumDimensions() const
    {
        return numDimensions;
    }

    hdps::Set* createSet() const;

    std::vector<float> data;
    unsigned int numDimensions = 1;
};

class IndexSet : public hdps::Set
{
public:
    IndexSet(hdps::CoreInterface* core, QString dataName) : Set(core, dataName) { }
    ~IndexSet() { }
    
    PointsPlugin* getData() const
    {
        return dynamic_cast<PointsPlugin*>(_core->requestData(getDataName()));
    }

    Set* copy() const override;

    std::vector<unsigned int> indices;
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
