#ifndef HDPS_PluginHierarchyModel_H
#define HDPS_PluginHierarchyModel_H

#include "event/EventListener.h"

#include <QAbstractItemModel>
#include <QMimeData>

#include <QVector>
#include <QIcon>

namespace hdps
{

class Core;
class PluginHierarchyItem;

/**
 * Underlying data model for a data hierarchy tree
 */
class PluginHierarchyModel : public QAbstractItemModel, public EventListener
{
    Q_OBJECT

public:
    explicit PluginHierarchyModel(Core* core, QObject* parent = nullptr);

    ~PluginHierarchyModel();

    /**
        * Mandatory override for QAbstractItemModel.
        */
    QVariant data(const QModelIndex& index, int role) const override;

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

    PluginHierarchyItem* getItem(const QModelIndex& index, int role) const;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /**
        * For the given item indices return the MIME data intended to pass information
        * in the case of e.g. dragging or dropping data.
        */
    QMimeData* mimeData(const QModelIndexList &indexes) const override;

private:
    //void setupModelData(DataManager& dataManager, PluginHierarchyItem* parent);

private:
    Core*                   _core;          /** Pointer to core */
    PluginHierarchyItem*    _rootItem;      /** Root node of the data hierarchy */
};

}

#endif // HDPS_PluginHierarchyModel_H
