#ifndef HDPS_HIERARCHY_MODEL_H
#define HDPS_HIERARCHY_MODEL_H

#include <QAbstractItemModel>
#include <QMimeData>

namespace hdps
{

class Core;
class DataHierarchyModelItem;

/**
 * Underlying data model for a data hierarchy tree
 */
class DataHierarchyModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit DataHierarchyModel(Core* core, QObject* parent = nullptr);

    ~DataHierarchyModel();

    /**
        * Mandatory override for QAbstractItemModel.
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
     * Mandatory override for QAbstractItemModel. Provides an index associated
     * to a particular data item at location (row, column).
     */
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

    /**
     * Mandatory override for QAbstractItemModel. Returns the index of the parent
     * of this item. If this item is not a child, an invalid index is returned.
     */
    QModelIndex parent(const QModelIndex& index) const override;

    /**
     * Mandatory override for QAbstractItemModel.
     * Returns the number of children of this parent item.
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * Mandatory override for QAbstractItemModel.
     * Returns number of columns associated with this parent item.
     */
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    Qt::DropActions supportedDragActions() const override;

    DataHierarchyModelItem* getItem(const QModelIndex& index, int role) const;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /**
     * For the given item indices return the MIME data intended to pass information
     * in the case of e.g. dragging or dropping data.
     */
    QMimeData* mimeData(const QModelIndexList &indexes) const override;

private:
    Core*                       _core;          /** Pointer to core */
    DataHierarchyModelItem*     _rootItem;      /** Root node of the data hierarchy */
};

}

#endif // HDPS_HIERARCHY_MODEL_H
