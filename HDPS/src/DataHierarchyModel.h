#pragma once

#include "DataManager.h"

#include <QAbstractItemModel>
#include <QMimeData>

#include <QVector>
#include <QIcon>

namespace hdps
{
    enum TypeOfData
    {
        SET, RAW_DATA
    };

    class DataHierarchyItem
    {
    public:
        explicit DataHierarchyItem(const QString kind, DataHierarchyItem* parent = nullptr);

        ~DataHierarchyItem();

        void setTypeOfData(TypeOfData typeOfData);
        void addChild(DataHierarchyItem* item);

        inline void setIcon(const QIcon &aicon)
        {
            _icon = aicon;
        }

        DataHierarchyItem* getParent();
        DataHierarchyItem* getChild(int row);
        const QIcon& getIcon() { return _icon; }

        QString getDataAtColumn(int column) const;
        int row() const;

        int childCount() const;
        int columnCount() const;

        QString serialize() const;

    private:
        /** Keep a pointer to parent node */
        DataHierarchyItem* _parentItem;

        /** Keep a list of pointers to child nodes */
        QVector<DataHierarchyItem*> _childItems;

        /** Actual data stored in node */
        QString _itemData;

        /** Type of the data stored in node */
        TypeOfData _typeOfData;

        /** Stored icon for this item */
        QIcon _icon;
    };

    /**
     * Underlying data model for a data hierarchy tree
     */
    class DataHierarchyModel : public QAbstractItemModel
    {
        Q_OBJECT
    public:
        explicit DataHierarchyModel(DataManager& dataManager, QObject* parent = nullptr);

        ~DataHierarchyModel();

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

        DataHierarchyItem* getItem(const QModelIndex& index, int role) const;

        Qt::ItemFlags flags(const QModelIndex &index) const override;

        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

        /**
         * For the given item indices return the MIME data intended to pass information
         * in the case of e.g. dragging or dropping data.
         */
        QMimeData* mimeData(const QModelIndexList &indexes) const override;

    private:
        void setupModelData(DataManager& dataManager, DataHierarchyItem* parent);

        /** Root node of the data hierarchy */
        DataHierarchyItem* _rootItem;

        /** Reference to data manager of core system to be able to query for data */
        DataManager& _dataManager;
    };
}
