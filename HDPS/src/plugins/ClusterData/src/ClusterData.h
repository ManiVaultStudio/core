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

class InfoAction;

struct Cluster
{
    std::vector<unsigned int> indices;

    QString _name;
    QColor _color;
    std::vector<float> _median;
    std::vector<float> _mean;
    std::vector<float> _stddev;
};

class ClusterData : public hdps::plugin::RawData
{
public:
    ClusterData(const hdps::plugin::PluginFactory* factory) : hdps::plugin::RawData(factory, ClusterType) { }
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

    void init() override;

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

    /** Get icon for the dataset */
    QIcon getIcon() const override;

    std::vector<unsigned int>       indices;
    QSharedPointer<InfoAction>      _infoAction;        /** Shared pointer to info action */
};

// =============================================================================
// Factory
// =============================================================================

class ClusterDataFactory : public hdps::plugin::RawDataFactory
{
    Q_INTERFACES(hdps::plugin::RawDataFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "hdps.ClusterData"
                      FILE  "ClusterData.json")
    
public:
    ClusterDataFactory(void) {}
    ~ClusterDataFactory(void) override {}
    
    hdps::plugin::RawData* produce() override;
};
