#pragma once

#include <RawData.h>
#include <Set.h>

#include <QString>
#include <QColor>

#include <vector>

// =============================================================================
// Data Type
// =============================================================================

const hdps::DataType ClusterType = hdps::DataType(QString("Clusters"));

// =============================================================================
// Raw Data
// =============================================================================

struct Cluster
{
    std::vector<unsigned int> indices;

    QString _name;
    QColor _color;
    std::vector<float> _median;
    std::vector<float> _mean;
    std::vector<float> _stddev;
};

class ClusterData : public hdps::RawData
{
public:
    ClusterData() : hdps::RawData("Clusters", ClusterType) { }
    ~ClusterData(void) override;
    
    void init() override;

    hdps::DataSet* createDataSet() const override;

    std::vector<Cluster>& getClusters()
    {
        return _clusters;
    }

    void addCluster(Cluster& cluster) {
        _clusters.push_back(cluster);
    }

private:
    std::vector<Cluster> _clusters;
};

// =============================================================================
// Cluster Set
// =============================================================================

class Clusters : public hdps::DataSet
{
public:
    Clusters(hdps::CoreInterface* core, QString dataName) : hdps::DataSet(core, dataName) { }
    ~Clusters() override { }

    std::vector<Cluster>& getClusters()
    {
        return getRawData<ClusterData>().getClusters();
    }

    void addCluster(Cluster& cluster)
    {
        getRawData<ClusterData>().addCluster(cluster);
    }

    DataSet* copy() const override
    {
        Clusters* clusters = new Clusters(_core, getDataName());
        clusters->setName(getName());
        clusters->indices = indices;
        return clusters;
    }

    QString createSubset(const QString parentSetName = "", const bool& visible = true) const override
    {
        const hdps::DataSet& selection = getSelection();

        return _core->createSubsetFromSelection(selection, *this, "Clusters", parentSetName, visible);
    }

    std::vector<unsigned int> indices;
};

// =============================================================================
// Factory
// =============================================================================

class ClusterDataFactory : public hdps::plugin::RawDataFactory
{
    Q_INTERFACES(hdps::plugin::RawDataFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.ClusterData"
                      FILE  "ClusterData.json")
    
public:
    ClusterDataFactory(void) {}
    ~ClusterDataFactory(void) override {}
    
    hdps::RawData* produce() override;
};
