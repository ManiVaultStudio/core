#ifndef CLUSTERS_PLUGIN_H
#define CLUSTERS_PLUGIN_H

#include <DataTypePlugin.h>
#include <PointsPlugin.h>
#include <Set.h>

#include <QString>
#include <vector>

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

class ClustersPlugin : public DataTypePlugin
{
public:
    ClustersPlugin() : DataTypePlugin("Clusters") { }
    ~ClustersPlugin(void);
    
    void init();

    hdps::Set* createSet() const;

    void addCluster(IndexSet* cluster) {
        clusters.push_back(cluster);
    }

    std::vector<IndexSet*> clusters;
};

class ClusterSet : public hdps::Set
{
public:
    ClusterSet() { }
    ~ClusterSet() { }

    Set* copy() const override;

    std::vector<unsigned int> indices;
};

// =============================================================================
// Factory
// =============================================================================

class ClustersPluginFactory : public DataTypePluginFactory
{
    Q_INTERFACES(hdps::plugin::DataTypePluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.ClustersPlugin"
                      FILE  "ClustersPlugin.json")
    
public:
    ClustersPluginFactory(void) {}
    ~ClustersPluginFactory(void) {}
    
    DataTypePlugin* produce();
};

#endif // CLUSTERS_PLUGIN_H
