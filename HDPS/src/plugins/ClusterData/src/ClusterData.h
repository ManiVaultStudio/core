// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "clusterdata_export.h"

#include "Cluster.h"

#include <event/EventListener.h>
#include <Application.h>

#include <RawData.h>
#include <Set.h>

#include <QString>
#include <QColor>
#include <QUuid>

#include <vector>

using namespace mv;

const mv::DataType ClusterType = mv::DataType(QString("Clusters"));

class InfoAction;

class CLUSTERDATA_EXPORT ClusterData : public mv::plugin::RawData
{
public:
    ClusterData(const mv::plugin::PluginFactory* factory);
    ~ClusterData(void) override;

    void init() override;

    /**
     * Create dataset for raw data
     * @param guid Globally unique dataset identifier (use only for deserialization)
     * @return Smart pointer to dataset
     */
    Dataset<DatasetImpl> createDataSet(const QString& guid = "") const override;

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

class CLUSTERDATA_EXPORT Clusters : public DatasetImpl
{
public:
    Clusters(QString dataName, const QString& guid = "") :
        DatasetImpl(dataName, guid)
    {
    }

    ~Clusters() override { }

    void init() override;

    QVector<Cluster>& getClusters()
    {
        return getRawData<ClusterData>()->getClusters();
    }

    const QVector<Cluster>& getClusters() const
    {
        return getRawData<ClusterData>()->getClusters();
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
        auto clusters = new Clusters(getRawDataName());

        clusters->setText(text());
        clusters->indices = indices;
        
        return Dataset<DatasetImpl>(clusters);
    }

    /**
     * Create subset from the current selection and specify where the subset will be placed in the data hierarchy
     * @param guiName Name of the subset in the GUI
     * @param parentDataSet Smart pointer to parent dataset in the data hierarchy (default is below the set)
     * @param visible Whether the subset will be visible in the UI
     * @return Smart pointer to the created subset
     */
    Dataset<DatasetImpl> createSubsetFromSelection(const QString& guiName, const Dataset<DatasetImpl>& parentDataSet = Dataset<DatasetImpl>(), const bool& visible = true) const  override
    {
        return Application::core()->createSubsetFromSelection(getSelection(), toSmartPointer(), guiName, parentDataSet, visible);
    }

    /**
     * Get plugin icon
     * @param color Icon color for flat (font) icons
     * @return Icon
     */
    QIcon getIcon(const QColor& color = Qt::black) const override;

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
    EventListener                   _eventListener;     /** Listen to HDPS events */
};

// =============================================================================
// Factory
// =============================================================================

class ClusterDataFactory : public mv::plugin::RawDataFactory
{
    Q_INTERFACES(mv::plugin::RawDataFactory mv::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "hdps.ClusterData"
                      FILE  "ClusterData.json")
    
public:
    ClusterDataFactory(void) {}
    ~ClusterDataFactory(void) override {}
    
    /**
     * Get plugin icon
     * @param color Icon color for flat (font) icons
     * @return Icon
     */
    QIcon getIcon(const QColor& color = Qt::black) const override;

    mv::plugin::RawData* produce() override;


};
