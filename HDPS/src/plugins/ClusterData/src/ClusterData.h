#pragma once

#include "clusterdata_export.h"

#include "Cluster.h"
#include "event/EventListener.h"

#include <RawData.h>
#include <Set.h>

#include <QString>
#include <QColor>
#include <QUuid>

#include <vector>

using namespace hdps;

const hdps::DataType ClusterType = hdps::DataType(QString("Clusters"));

class InfoAction;

class CLUSTERDATA_EXPORT ClusterData : public hdps::plugin::RawData
{
public:
    ClusterData(const hdps::plugin::PluginFactory* factory);
    ~ClusterData(void) override;

    void init() override;

    /** Create dataset for raw data */
    Dataset<DatasetImpl> createDataSet() const override;

    /** Returns reference to the clusters */
    QVector<Cluster>& getClusters();

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

    /**
     * Get cluster index by name
     * @param clusterName Name of the cluster to search for
     * @return Index of the cluster (-1 if not found)
     */
    std::int32_t getClusterIndex(const QString& clusterName) const;

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

private:
    QVector<Cluster>    _clusters;      /** Clusters data */
};

// =============================================================================
// Cluster Set
// =============================================================================

class CLUSTERDATA_EXPORT Clusters : public DatasetImpl, public hdps::EventListener
{
public:
    Clusters(CoreInterface* core, QString dataName) : DatasetImpl(core, dataName) { }
    ~Clusters() override { }

    void init() override;

    QVector<Cluster>& getClusters()
    {
        return getRawData<ClusterData>().getClusters();
    }

    const QVector<Cluster>& getClusters() const
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

    /**
     * Get a copy of the dataset
     * @return Smart pointer to copy of dataset
     */
    Dataset<DatasetImpl> copy() const override
    {
        auto clusters = new Clusters(_core, getRawDataName());

        clusters->setGuiName(getGuiName());
        clusters->indices = indices;
        
        return Dataset<DatasetImpl>(clusters);
    }

    /**
     * Create subset and specify where the subset will be placed in the data hierarchy
     * @param guiName Name of the subset in the GUI
     * @param parentDataSet Smart pointer to parent dataset in the data hierarchy (default is below the set)
     * @param visible Whether the subset will be visible in the UI
     * @return Smart pointer to the created subset
     */
    Dataset<DatasetImpl> createSubset(const QString& guiName, const Dataset<DatasetImpl>& parentDataSet = Dataset<DatasetImpl>(), const bool& visible = true) const  override
    {
        return _core->createSubsetFromSelection(getSelection(), toSmartPointer(), guiName, parentDataSet, visible);
    }

    /**
     * Get the dataset icon
     * @param size Size of the icon
     * @return Dataset icon
     */
    QIcon getIcon(const QSize& size = QSize(16, 16)) const override;

public: // Selection

    /**
     * Get selection indices
     * @return Selection indices
     */
    std::vector<std::uint32_t>& getSelectionIndices() override;

    /**
     * Select by indices
     * @param indices Selection indices
     */
    void setSelectionIndices(const std::vector<std::uint32_t>& indices) override;

    /**
     * Get selection names
     * @return Selected cluster names
     */
    QStringList getSelectionNames() const;

    /**
     * Select by name
     * @param clusterNames Cluster names
     */
    void setSelectionNames(const QStringList& clusterNames);

    /** Determines whether items can be selected */
    bool canSelect() const override;

    /** Determines whether all items can be selected */
    bool canSelectAll() const override;

    /** Determines whether there are any items which can be deselected */
    bool canSelectNone() const override;

    /** Determines whether the item selection can be inverted (more than one) */
    bool canSelectInvert() const override;

    /** Select all items */
    void selectAll() override;

    /** Deselect all items */
    void selectNone() override;

    /** Invert item selection */
    void selectInvert() override;

    /** Gets concatenated indices for all selected clusters */
    std::vector<std::uint32_t> getSelectedIndices() const;

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;


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

    /**
     * Get the plugin icon
     * @param size Size of the icon
     * @return Plugin icon
     */
    QIcon getIcon(const QSize& size = QSize(16, 16)) const override;

    hdps::plugin::RawData* produce() override;
};
