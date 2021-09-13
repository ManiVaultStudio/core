#pragma once

#include "clusterdata_export.h"

#include "ClusterDataVisitor.h"
#include "Cluster.h"

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

class CLUSTERDATA_EXPORT ClusterData : public hdps::plugin::RawData, public hdps::util::Visitable<ClusterDataVisitor>
{
public:
    ClusterData(const hdps::plugin::PluginFactory* factory);
    ~ClusterData(void) override;
    
    void init() override;

    hdps::DataSet* createDataSet() const override;

    /** Returns reference to the clusters */
    std::vector<Cluster>& getClusters();

    /** Accept cluster data visitor for visiting
     * @param visitor Reference to visitor that will visit this component
     */
    void accept(ClusterDataVisitor* visitor) const override;

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

    const std::vector<Cluster>& getClusters() const
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

    /**
     * Create subset
     * @param subsetName Name of the subset
     * @param parentSetName Name of the parent dataset
     * @param visible Whether the subset will be visible in the UI
     */
    QString createSubset(const QString subsetName = "subset", const QString parentSetName = "", const bool& visible = true) const override
    {
        const hdps::DataSet& selection = getSelection();

        return _core->createSubsetFromSelection(selection, *this, "Clusters", parentSetName, visible);
    }

    /** Get icon for the dataset */
    QIcon getIcon() const override;

    /** Loads cluster from variant list */
    void fromVariant(const QVariant& variant);

    /** Returns a variant representation of the clusters */
    QVariant toVariant() const;

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
    
    /** Returns the plugin icon */
    QIcon getIcon() const override;

    hdps::plugin::RawData* produce() override;
};
