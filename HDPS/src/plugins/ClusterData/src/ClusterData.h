#pragma once

#include <RawData.h>
#include <Set.h>

#include <QString>
#include <QColor>

#include <vector>

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

struct Cluster
{
    std::vector<unsigned int> indices;

    QColor _color;
    std::vector<float> _median;
    std::vector<float> _mean;
    std::vector<float> _stddev;
};

class ClusterData : public RawData
{
public:
    ClusterData() : RawData("Cluster") { }
    ~ClusterData(void) override;
    
    void init() override;

    hdps::Set* createSet() const override;

    void addCluster(Cluster& cluster) {
        clusters.push_back(cluster);
    }

    std::vector<Cluster> clusters;
};

// =============================================================================
// Factory
// =============================================================================

class ClusterDataFactory : public RawDataFactory
{
    Q_INTERFACES(hdps::plugin::RawDataFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.ClusterData"
                      FILE  "ClusterData.json")
    
public:
    ClusterDataFactory(void) {}
    ~ClusterDataFactory(void) override {}
    
    RawData* produce() override;
};
