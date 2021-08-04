#include "DataHierarchyModel.h"
#include "DataHierarchyModelItem.h"
#include "DataHierarchyManager.h"
#include "Plugin.h"
#include "Core.h"
#include "AnalysisPlugin.h"

#include <QDebug>

namespace hdps
{

PluginHierarchyModel::PluginHierarchyModel(Core* core, QObject* parent) :
    QAbstractItemModel(parent),
    EventListener(),
    _core(core),
    _rootItem(new DataHierarchyModelItem())
{
    setEventCore(reinterpret_cast<CoreInterface*>(_core));

    connect(&_core->getDataHierarchyManager(), &DataHierarchyManager::itemAdded, this, [this](const DataHierarchyManager::DataHierarchyItem& dataHierarchyItem) {
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

            beginInsertRows(parentModelIndex, numberOfChildren, numberOfChildren + 1);
            {
                dataItem->addChild(new DataHierarchyModelItem(dataHierarchyItem.getDatasetName()));
            }
            endInsertRows();
        }
        emit layoutChanged();
    });

    registerAnalysisEvent([this](const hdps::AnalysisEvent& analysisEvent) {
        const auto outputDatasetName    = analysisEvent.getAnalysisPlugin()->getOutputDatasetName();
        const auto outputDatasetIndex   = match(index(0, 0), Qt::DisplayRole, outputDatasetName, 1, Qt::MatchFlag::MatchRecursive).first();
        
        switch (analysisEvent.getType())
        {
            case EventType::AnalysisStarted:
            {
                setData(outputDatasetIndex.siblingAtColumn(static_cast<std::int32_t>(DataHierarchyModelItem::Column::Analyzing)), true);
                break;
            }

            case EventType::AnalysisProgressSection:
            {
                auto event = static_cast<const AnalysisProgressSectionEvent*>(&analysisEvent);
                setData(outputDatasetIndex.siblingAtColumn(static_cast<std::int32_t>(DataHierarchyModelItem::Column::Description)), event->getSection());
                break;
            }

            case EventType::AnalysisProgressPercentage:
            {
                auto event = static_cast<const AnalysisProgressPercentageEvent*>(&analysisEvent);
                setData(outputDatasetIndex.siblingAtColumn(static_cast<std::int32_t>(DataHierarchyModelItem::Column::Progress)), event->getPercentage());
                break;
            }

            case EventType::AnalysisFinished:
            case EventType::AnalysisAborted:
            {
                setData(outputDatasetIndex.siblingAtColumn(static_cast<std::int32_t>(DataHierarchyModelItem::Column::Description)), "");
                setData(outputDatasetIndex.siblingAtColumn(static_cast<std::int32_t>(DataHierarchyModelItem::Column::Progress)), 0);
                setData(outputDatasetIndex.siblingAtColumn(static_cast<std::int32_t>(DataHierarchyModelItem::Column::Analyzing)), false);
                break;
            }

            default:
                break;
        }
    });
}

PluginHierarchyModel::~PluginHierarchyModel()
{
    delete _rootItem;
}

QVariant PluginHierarchyModel::data(const QModelIndex& index, int role) const
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

bool PluginHierarchyModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
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

    emit dataChanged(index.siblingAtColumn(static_cast<std::int32_t>(DataHierarchyModelItem::Column::_start)), index.siblingAtColumn(static_cast<std::int32_t>(DataHierarchyModelItem::Column::_end)));

    return true;
}

QModelIndex PluginHierarchyModel::index(int row, int column, const QModelIndex& parent) const
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

QModelIndex PluginHierarchyModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    auto childItem  = static_cast<DataHierarchyModelItem*>(index.internalPointer());
    auto parentItem = childItem->getParent();

    if (parentItem == nullptr || parentItem == _rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int PluginHierarchyModel::rowCount(const QModelIndex& parent) const
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

int PluginHierarchyModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return static_cast<DataHierarchyModelItem*>(parent.internalPointer())->getNumColumns();

    return _rootItem->getNumColumns();
}

Qt::DropActions PluginHierarchyModel::supportedDragActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

DataHierarchyModelItem* PluginHierarchyModel::getItem(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return nullptr;

    if (role != Qt::DisplayRole)
        return nullptr;

    return static_cast<DataHierarchyModelItem*>(index.internalPointer());
}

Qt::ItemFlags PluginHierarchyModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    
    return Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | QAbstractItemModel::flags(index);
}

QVariant PluginHierarchyModel::headerData(int section, Qt::Orientation orientation, int role) const
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

QMimeData* PluginHierarchyModel::mimeData(const QModelIndexList &indexes) const
{
    QVector<DataHierarchyModelItem*> items;

    foreach(const QModelIndex &index, indexes)
        items.push_back(getItem(index, Qt::DisplayRole));

    QMimeData* mimeData = QAbstractItemModel::mimeData(indexes);

    mimeData->setText(items[0]->serialize());

    return mimeData;
}

/*
void PluginHierarchyModel::setupModelData(DataManager& dataManager, PluginHierarchyItem* parent)
{
    for (auto& setPair : dataManager.allSets())
    {
        QString setName = setPair.first;
        auto& set = setPair.second;

        //RawData& rawData = dataManager.getRawData(set->getDataName());

        PluginHierarchyItem* setItem = new PluginHierarchyItem(setName, set->getDataType(), _rootItem);
            
        _rootItem->addChild(setItem);
    }
}
*/

}
