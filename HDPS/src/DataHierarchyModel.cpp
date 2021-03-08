#include "DataHierarchyModel.h"

#include <QDebug>

namespace hdps
{
    /////////////////////////
    // Data Hierarchy Item //
    /////////////////////////
    DataHierarchyItem::DataHierarchyItem(QString setName, DataType dataType, DataHierarchyItem* parent) :
        _setName(setName),
        _dataType(dataType),
        _parentItem(parent)
    {
        //if (dataType == "Points")
        //    setIcon(QIcon(":/icons/DataIcon.png"));
        //if (kind == "Text")
        //    setIcon(QIcon(":/icons/TextIcon.png"));
        //if (kind == "Color")
        //    setIcon(QIcon(":/icons/ColorIcon.png"));
        //if (kind == "Cluster")
        //    setIcon(QIcon(":/icons/ClusterIcon.png"));
    }

    DataHierarchyItem::~DataHierarchyItem()
    {
        qDeleteAll(_childItems);
    }

    void DataHierarchyItem::addChild(DataHierarchyItem* item)
    {
        _childItems.append(item);
    }

    DataHierarchyItem* DataHierarchyItem::getParent()
    {
        return _parentItem;
    }

    DataHierarchyItem* DataHierarchyItem::getChild(int row)
    {
        if (row < 0 || row >= _childItems.size())
            return nullptr;
        return _childItems[row];
    }

    // String that gets displayed in the data hierarchy at the given column
    QString DataHierarchyItem::getDataAtColumn(int column) const
    {
        return _setName;
    }

    int DataHierarchyItem::row() const
    {
        if (_parentItem)
            return _parentItem->_childItems.indexOf(const_cast<DataHierarchyItem*>(this));
        return 0;
    }

    int DataHierarchyItem::childCount() const
    {
        return _childItems.count();
    }

    int DataHierarchyItem::columnCount() const
    {
        return 1;
    }

    QString DataHierarchyItem::serialize() const
    {
        return _setName + "\n" + _dataType;
    }

    //////////////////////////
    // Data Hierarchy Model //
    //////////////////////////
    DataHierarchyModel::DataHierarchyModel(DataManager& dataManager, QObject* parent) :
        QAbstractItemModel(parent),
        _dataManager(dataManager)
    {
        _rootItem = new DataHierarchyItem("Data Hierarchy", DataType("Root"));
        setupModelData(dataManager, _rootItem);
    }

    DataHierarchyModel::~DataHierarchyModel()
    {
        delete _rootItem;
    }

    QVariant DataHierarchyModel::data(const QModelIndex& index, int role) const
    {
        if (!index.isValid())
            return QVariant();

        DataHierarchyItem* item = static_cast<DataHierarchyItem*>(index.internalPointer());
        if (role == Qt::DecorationRole) {
            return item->getIcon();
        }
        if (role == Qt::ToolTipRole)
            return item->getDataAtColumn(0);

        if (role != Qt::DisplayRole)
            return QVariant();

        return item->getDataAtColumn(index.column());
    }

    QModelIndex DataHierarchyModel::index(int row, int column, const QModelIndex& parent) const
    {
        if (!hasIndex(row, column, parent))
            return QModelIndex();

        DataHierarchyItem* parentItem;

        if (!parent.isValid())
            parentItem = _rootItem;
        else
            parentItem = static_cast<DataHierarchyItem*>(parent.internalPointer());

        DataHierarchyItem* childItem = parentItem->getChild(row);
        if (childItem)
            return createIndex(row, column, childItem);
        return QModelIndex();
    }

    QModelIndex DataHierarchyModel::parent(const QModelIndex& index) const
    {
        if (!index.isValid())
            return QModelIndex();

        DataHierarchyItem* childItem = static_cast<DataHierarchyItem*>(index.internalPointer());
        DataHierarchyItem* parentItem = childItem->getParent();

        if (parentItem == _rootItem)
            return QModelIndex();

        return createIndex(parentItem->row(), 0, parentItem);
    }

    int DataHierarchyModel::rowCount(const QModelIndex& parent) const
    {
        DataHierarchyItem* parentItem;
        if (parent.column() > 0)
            return 0;

        if (!parent.isValid())
            parentItem = _rootItem;
        else
            parentItem = static_cast<DataHierarchyItem*>(parent.internalPointer());

        return parentItem->childCount();
    }

    int DataHierarchyModel::columnCount(const QModelIndex& parent) const
    {
        if (parent.isValid())
            return static_cast<DataHierarchyItem*>(parent.internalPointer())->columnCount();
        return _rootItem->columnCount();
    }

    Qt::DropActions DataHierarchyModel::supportedDragActions() const
    {
        return Qt::CopyAction | Qt::MoveAction;
    }

    DataHierarchyItem* DataHierarchyModel::getItem(const QModelIndex& index, int role) const
    {
        if (!index.isValid())
            return nullptr;

        if (role != Qt::DisplayRole)
            return nullptr;

        DataHierarchyItem* item = static_cast<DataHierarchyItem*>(index.internalPointer());

        return item;
    }

    Qt::ItemFlags DataHierarchyModel::flags(const QModelIndex &index) const
    {
        // If the index is negative or doesn't belong to this model, return no item flags
        if (!index.isValid())
        {
            return Qt::NoItemFlags;
        }
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | QAbstractItemModel::flags(index);
    }


    QVariant DataHierarchyModel::headerData(int section, Qt::Orientation orientation, int role) const
    {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
            return _rootItem->getDataAtColumn(section);

        return QVariant();
    }

    QMimeData* DataHierarchyModel::mimeData(const QModelIndexList &indexes) const
    {
        QVector<DataHierarchyItem*> items;
        foreach(const QModelIndex &index, indexes)
            items.push_back(getItem(index, Qt::DisplayRole));

        QMimeData* mimeData = QAbstractItemModel::mimeData(indexes);

        mimeData->setText(items[0]->serialize());
        return mimeData;
    }

    void DataHierarchyModel::setupModelData(DataManager& dataManager, DataHierarchyItem* parent)
    {
        for (auto& setPair : dataManager.allSets())
        {
            QString setName = setPair.first;
            auto& set = setPair.second;

            //RawData& rawData = dataManager.getRawData(set->getDataName());

            DataHierarchyItem* setItem = new DataHierarchyItem(setName, set->getDataType(), _rootItem);
            
            _rootItem->addChild(setItem);
        }
    }
}
