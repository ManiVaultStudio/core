#include "DataHierarchyModel.h"
#include "DataHierarchyModelItem.h"
#include "DataHierarchyManager.h"
#include "Plugin.h"
#include "Core.h"

#include <QDebug>

namespace hdps
{

DataHierarchyModel::DataHierarchyModel(Core* core, QObject* parent) :
    QAbstractItemModel(parent),
    _core(core),
    _rootItem(new DataHierarchyModelItem(nullptr))
{
    connect(&_core->getDataHierarchyManager(), &DataHierarchyManager::hierarchyItemAdded, this, [this](DataHierarchyItem& dataHierarchyItem) {
        if (dataHierarchyItem.isHidden())
            return;

        DataSet& dataset = _core->requestData(dataHierarchyItem.getDatasetName());

        QModelIndex parentModelIndex;

        const auto parentDatasetName = dataHierarchyItem.getParent();

        if (parentDatasetName.isEmpty())
            parentModelIndex = index(0, 0);
        else
            parentModelIndex = match(index(0, 0), Qt::DisplayRole, parentDatasetName, 1, Qt::MatchFlag::MatchRecursive).first();

        auto dataItem = !parentModelIndex.isValid() ? _rootItem : getItem(parentModelIndex, Qt::DisplayRole);

        emit layoutAboutToBeChanged();
        {
            const auto numberOfChildren = dataHierarchyItem.getNumberOfChildren();

            //qDebug() << dataHierarchyItem;
            //qDebug() << numberOfChildren;

            beginInsertRows(parentModelIndex, numberOfChildren, numberOfChildren + 1);
            {
                dataItem->addChild(new DataHierarchyModelItem(&dataHierarchyItem));
            }
            endInsertRows();
        }
        emit layoutChanged();

        connect(&dataHierarchyItem, &DataHierarchyItem::descriptionChanged, this, [this, &dataHierarchyItem](const QString& description) {
            const auto outputDatasetName    = dataHierarchyItem.getDatasetName();
            const auto outputDatasetIndex   = match(index(0, 0), Qt::DisplayRole, outputDatasetName, 1, Qt::MatchFlag::MatchRecursive).first();

            setData(outputDatasetIndex.siblingAtColumn(static_cast<std::int32_t>(DataHierarchyModelItem::Column::Description)), description);
        });

        connect(&dataHierarchyItem, &DataHierarchyItem::progressChanged, this, [this, &dataHierarchyItem](const float& progress) {
            const auto outputDatasetName    = dataHierarchyItem.getDatasetName();
            const auto outputDatasetIndex   = match(index(0, 0), Qt::DisplayRole, outputDatasetName, 1, Qt::MatchFlag::MatchRecursive).first();

            setData(outputDatasetIndex.siblingAtColumn(static_cast<std::int32_t>(DataHierarchyModelItem::Column::Progress)), progress);
            setData(outputDatasetIndex.siblingAtColumn(static_cast<std::int32_t>(DataHierarchyModelItem::Column::Analyzing)), progress > 0.0f);
        });
    });
}

DataHierarchyModel::~DataHierarchyModel()
{
    delete _rootItem;
}

QVariant DataHierarchyModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto item = static_cast<DataHierarchyModelItem*>(index.internalPointer());

    if (role == Qt::DecorationRole)
        return item->getIconAtColumn(index.column());

    if (role == Qt::ToolTipRole)
        return item->getDataAtColumn(index.column());

    if (role != Qt::DisplayRole)
        return QVariant();

    return item->getDataAtColumn(index.column());
}

bool DataHierarchyModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
{
    const auto column = static_cast<DataHierarchyModelItem::Column>(index.column());

    auto dataHierarchyItem = static_cast<DataHierarchyModelItem*>((void*)index.internalPointer());
    
    switch (column) {
        case DataHierarchyModelItem::Column::Name:
            break;

        case DataHierarchyModelItem::Column::Description:
            dataHierarchyItem->setProgressSection(value.toString());
            break;

        case DataHierarchyModelItem::Column::Progress:
            dataHierarchyItem->setProgressPercentage(value.toFloat());
            break;

        case DataHierarchyModelItem::Column::Analyzing:
            dataHierarchyItem->setAnalyzing(value.toBool());
            break;

        default:
            break;
    }

    emit dataChanged(index, index);

    return true;
}

QModelIndex DataHierarchyModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    DataHierarchyModelItem* parentItem;

    if (!parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<DataHierarchyModelItem*>(parent.internalPointer());

    auto childItem = parentItem->getChild(row);

    if (childItem)
        return createIndex(row, column, childItem);

    return QModelIndex();
}

QModelIndex DataHierarchyModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    auto childItem  = static_cast<DataHierarchyModelItem*>(index.internalPointer());
    auto parentItem = childItem->getParent();

    if (parentItem == nullptr || parentItem == _rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int DataHierarchyModel::rowCount(const QModelIndex& parent) const
{
    DataHierarchyModelItem* parentItem;

    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<DataHierarchyModelItem*>(parent.internalPointer());

    return parentItem->getNumChildren();
}

int DataHierarchyModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return static_cast<DataHierarchyModelItem*>(parent.internalPointer())->getNumColumns();

    return _rootItem->getNumColumns();
}

Qt::DropActions DataHierarchyModel::supportedDragActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

DataHierarchyModelItem* DataHierarchyModel::getItem(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return nullptr;

    if (role != Qt::DisplayRole)
        return nullptr;

    return static_cast<DataHierarchyModelItem*>(index.internalPointer());
}

Qt::ItemFlags DataHierarchyModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    
    return Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | QAbstractItemModel::flags(index);
}

QVariant DataHierarchyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (static_cast<DataHierarchyModelItem::Column>(section))
        {
            case DataHierarchyModelItem::Column::Name:
                return "Name";

            case DataHierarchyModelItem::Column::Description:
                return "Description";

            case DataHierarchyModelItem::Column::Analyzing:
            case DataHierarchyModelItem::Column::Progress:
                return "";

            default:
                break;
        }

        return "";
    }

    return QVariant();
}

QMimeData* DataHierarchyModel::mimeData(const QModelIndexList &indexes) const
{
    QVector<DataHierarchyModelItem*> items;

    foreach(const QModelIndex &index, indexes)
        items.push_back(getItem(index, Qt::DisplayRole));

    QMimeData* mimeData = QAbstractItemModel::mimeData(indexes);

    mimeData->setText(items[0]->serialize());

    return mimeData;
}

}
