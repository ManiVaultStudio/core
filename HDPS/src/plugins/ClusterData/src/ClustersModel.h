// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "clusterdata_export.h"

#include "Cluster.h"

#include <actions/Actions.h>

using namespace mv;

/**
 * Clusters model class
 *
 * List model class for storing clusters
 *
 * @author Thomas Kroes
 */
class CLUSTERDATA_EXPORT ClustersModel : public QAbstractListModel
{
    Q_OBJECT

public:

    /** Columns */
    enum class Column {
        Color,              /** Color of the cluster */
        Name,               /** Name of the cluster */
        ID,                 /** Identifier of the cluster */
        NumberOfIndices,    /** Number of indices in the cluster */
        ModifiedByUser,     /** Whether the cluster is modified by the user */

        /** Number of columns */
        Count
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    ClustersModel(QObject* parent = nullptr);

    /**
     * Get the number of row
     * @param parent Parent model index
     * @return Number of rows in the model
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * Get the number of columns
     * @param parent Parent model index
     * @return Number of columns in the model
     */
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * Get data
     * @param index Model index to query
     * @param role Data role
     * @return Data
     */
    QVariant data(const QModelIndex& index, int role) const override;

    /**
     * Sets the data value for the given model index and data role
     * @param index Model index
     * @param value Data value in variant form
     * @param role Data role
     * @return Whether the data was properly set or not
     */
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    /**
     * Get header data
     * @param section Section
     * @param orientation Orientation
     * @param role Data role
     * @return Header
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /**
     * Mandatory override for QAbstractItemModel. Provides an index associated
     * to a particular data item at location (row, column).
     */
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

    /**
     * Get item flags
     * @param index Model index
     * @return Item flags
     */
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    /** Get clusters */
    QVector<Cluster>& getClusters();

    /**
     * Set clusters
     * @param clusters Pointer to clusters
     */
    void setClusters(const QVector<Cluster>& clusters);

    /**
     * Remove clusters by their unique identifiers
     * @param ids Unique identifiers of the clusters to remove
     */
    void removeClustersById(const QStringList& ids);

    /**
     * Set cluster prefix
     * @param clusterPrefix Cluster name prefix
     */
    void setClusterPrefix(const QString& clusterPrefix);

    /**
     * Colorize clusters by pseudo-random colors
     * @param randomSeed Random seed for pseudo-random colors
     */
    void colorizeClusters(std::int32_t randomSeed = 0);

    /**
     * Colorize clusters by color map
     * @param colorMapImage Color map image
     */
    void colorizeClusters(const QImage& colorMapImage);

    /**
     * Get the number of clusters that have been modified by the user
     * @return Number of user modified clusters
     */
    std::uint32_t getNumberOfUserModifiedClusters() const;

    /**
     * Get whether the model has user modifications
     * @return Whether the model has user modifications
     */
    bool hasUserModifications() const;

    /**
     * Get whether all cluster names start with the supplied prefix
     * @param prefix Cluster prefix
     */
    bool doAllClusterNamesStartWith(const QString& prefix);

protected:

    /** Get whether user modified clusters may be overridden by bulk operations (prefix change etc.) */
    bool mayOverrideUserInput();

private:

    /**
     * Get color icon
     * @param color Color of the icon
     * @return Icon
     */
    QIcon getColorIcon(const QColor& color) const;

    /** Apply cluster prefix to all clusters*/
    void applyClusterPrefixToAllClusters();

public:
    QVector<Cluster>    _clusters;          /** Vector of clusters */
    QVector<bool>       _modifiedByUser;    /** Boolean for each cluster, denoting whether it has been modified by the user */
    QString             _clusterPrefix;     /** Cluster prefix */
};