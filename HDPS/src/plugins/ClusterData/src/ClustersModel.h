#pragma once

#include "actions/Actions.h"

namespace hdps {
    class CoreInterface;
}

struct Cluster;

using namespace hdps;

/**
 * Clusters model class
 *
 * Model class for display of clusters
 *
 * @author Thomas Kroes
 */
class ClustersModel : public QAbstractListModel
{
    Q_OBJECT

public:

    /** Columns */
    enum class Column {
        Color,              /** Color of the cluster */
        Name,               /** Name of the cluster */
        ID,                 /** Identifier of the cluster */
        NumberOfIndices,     /** Number of points in the cluster */

        /** Number of columns */
        _Count
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    ClustersModel(QObject* parent);

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

    /**
     * Get cluster
     * @param row Row index
     * @return Cluster
     */
    Cluster& getCluster(const std::int32_t& row);

    /**
     * Set clusters
     * @param clusters Pointer to clusters
     */
    void setClusters(std::vector<Cluster>* clusters);

private:

    /**
     * Get decoration role (icon)
     * @param color Color of the icon
     * @return Icon
     */
    QIcon getDecorationRole(const QColor& color) const;

public:
    std::vector<Cluster>*    _clusters;     /** List of clusters from cluster data */
};