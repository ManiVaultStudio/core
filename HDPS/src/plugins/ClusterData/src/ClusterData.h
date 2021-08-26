#pragma once

#include "clusterdata_export.h"

#include <RawData.h>
#include <Set.h>

#include <QString>
#include <QColor>
#include <QUuid>

#include <vector>

// =============================================================================
// Data Type
// =============================================================================

const hdps::DataType ClusterType = hdps::DataType(QString("Clusters"));

// =============================================================================
// Raw Data
// =============================================================================

class InfoAction;

struct CLUSTERDATA_EXPORT Cluster
{
    /** Constructor */
    Cluster() :
        _name(""),
        _id(QUuid::createUuid().toString()),
        _color(Qt::gray),
        _median(),
        _mean(),
        _stddev()
    {
    }

    /**
     * Comparison operator for two clusters (compares the internal identifiers)
     * @param rhs Right hand sign of the comparison
     */
    bool operator==(const Cluster& rhs) const {
        if (rhs._name != _name)
            return false;

        if (rhs._id != _id)
            return false;

        if (rhs._color != _color)
            return false;

        if (rhs._indices != _indices)
            return false;

        if (rhs._median != _median)
            return false;

        if (rhs._mean != _mean)
            return false;

        if (rhs._stddev != _stddev)
            return false;

        return true;
    }

    QString                     _name;          /** GUI name */
    QString                     _id;            /** Unique cluster name */
    QColor                      _color;         /** Cluster color */
    std::vector<unsigned int>   _indices;       /** Indices contained by the cluster */
    std::vector<float>          _median;        /** Median values */
    std::vector<float>          _mean;          /** Mean values */
    std::vector<float>          _stddev;        /** Standard deviation values */
};

class CLUSTERDATA_EXPORT ClusterData : public hdps::plugin::RawData
{
public:
    ClusterData(const hdps::plugin::PluginFactory* factory);
    ~ClusterData(void) override;
    
    void init() override;

    hdps::DataSet* createDataSet() const override;

    /** Returns reference to the clusters */
    std::vector<Cluster>& getClusters();

    /**
     * Adds a cluster
     * @param cluster Cluster to add
     */
    void addCluster(Cluster& cluster);

    /**
     * Removes a cluster by its unique identifier
     * @param id Unique identifier of the cluster to remove
     */
    void removeClusterById(const QString& id);

    /**
     * Remove clusters by their unique identifiers
     * @param ids Unique identifiers of the clusters to remove
     */
    void removeClustersById(const QStringList& ids);

private:
    std::vector<Cluster>    _clusters;      /** Clusters data */
};

// =============================================================================
// Cluster Set
// =============================================================================

class CLUSTERDATA_EXPORT Clusters : public hdps::DataSet
{
public:
    Clusters(hdps::CoreInterface* core, QString dataName) : hdps::DataSet(core, dataName) { }
    ~Clusters() override { }

    void init() override;

    std::vector<Cluster>& getClusters()
    {
        return getRawData<ClusterData>().getClusters();
    }

    /**
     * Adds a cluster
     * @param cluster Cluster to add
     */
    void addCluster(Cluster& cluster);

    /**
     * Removes a cluster by its unique identifier
     * @param id Unique identifier of the cluster to remove
     */
    void removeClusterById(const QString& id);

    /**
     * Remove clusters by their unique identifiers
     * @param ids Unique identifiers of the clusters to remove
     */
    void removeClustersById(const QStringList& ids);

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
