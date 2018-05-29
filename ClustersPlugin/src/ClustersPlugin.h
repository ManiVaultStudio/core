#ifndef CLUSTERS_PLUGIN_H
#define CLUSTERS_PLUGIN_H

#include <RawData.h>
#include <PointsPlugin.h>
#include <Set.h>

#include <QString>
#include <vector>

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

class ClustersPlugin : public RawData
{
public:
    ClustersPlugin() : RawData("Clusters") { }
    ~ClustersPlugin(void);
    
    void init();

    hdps::Set* createSet() const;

    void addCluster(IndexSet* cluster) {
        clusters.push_back(cluster);
    }

    std::vector<IndexSet*> clusters;
};

struct Cluster {
    std::vector<unsigned int> indices;

    std::vector<float> _median;
    std::vector<float> _mean;
    std::vector<float> _stddev;
};

class ClusterSet : public hdps::Set
{
public:
    ClusterSet(hdps::CoreInterface* core, QString dataName) : Set(core, dataName) { }
    ~ClusterSet() { }

    ClustersPlugin& getData() const
    {
        return dynamic_cast<ClustersPlugin&>(_core->requestData(getDataName()));
    }

    Set* copy() const override;

    std::vector<unsigned int> indices;
};

// =============================================================================
// Factory
// =============================================================================

class ClustersPluginFactory : public RawDataFactory
{
    Q_INTERFACES(hdps::plugin::RawDataFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.ClustersPlugin"
                      FILE  "ClustersPlugin.json")
    
public:
    ClustersPluginFactory(void) {}
    ~ClustersPluginFactory(void) {}
    
    RawData* produce();
};

#endif // CLUSTERS_PLUGIN_H
