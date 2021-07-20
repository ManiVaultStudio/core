#include "PluginHierarchyModel.h"
#include "PluginHierarchyItem.h"
#include "Plugin.h"
#include "DataExportAction.h"
#include "DataAnalysisAction.h"

#include <QDebug>

namespace hdps
{

PluginHierarchyModel::PluginHierarchyModel(Core* core, QObject* parent) :
    QAbstractItemModel(parent),
    EventListener(),
    _core(core),
    _rootItem(new PluginHierarchyItem())
{
    setEventCore(reinterpret_cast<CoreInterface*>(_core));

    registerPluginEvent([this](hdps::PluginEvent* pluginEvent) {
        if (pluginEvent->getType() == EventType::PluginAdded) {

            switch (pluginEvent->_plugin->getType())
            {
                case plugin::Type::ANALYSIS:
                {
                    //pluginEvent->_plugin->exposeAction(new DataAction(this));
                    break;
                }

                case plugin::Type::DATA:
                {
                    beginInsertRows(QModelIndex(), _rootItem->childCount(), _rootItem->childCount() + 1);
                    {
                        pluginEvent->_plugin->exposeAction(new DataExportAction(this));
                        pluginEvent->_plugin->exposeAction(new DataAnalysisAction(this));

                        _rootItem->addChild(new PluginHierarchyItem(pluginEvent->_plugin));
                    }
                    endInsertRows();

                    break;
                }

                case plugin::Type::WRITER:
                {
                    break;
                }
            }

            
            //insertRow(1);
        }
    });

    /*
    registerDataEvent(
        pluginEvent->_plugin->exposeAction(new gui::TriggerAction(this, "Mean-shift"));
    );
    */
}

PluginHierarchyModel::~PluginHierarchyModel()
{
    delete _rootItem;
}

QVariant PluginHierarchyModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto item = static_cast<PluginHierarchyItem*>(index.internalPointer());

    if (role == Qt::DecorationRole)
        return item->getIcon();

    if (role == Qt::ToolTipRole)
        return item->getDataAtColumn(0);

    if (role != Qt::DisplayRole)
        return QVariant();

    return item->getDataAtColumn(index.column());
}

QModelIndex PluginHierarchyModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    PluginHierarchyItem* parentItem;

    if (!parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<PluginHierarchyItem*>(parent.internalPointer());

    PluginHierarchyItem* childItem = parentItem->getChild(row);

    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex PluginHierarchyModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    PluginHierarchyItem* childItem = static_cast<PluginHierarchyItem*>(index.internalPointer());
    PluginHierarchyItem* parentItem = childItem->getParent();

    if (parentItem == nullptr || parentItem == _rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int PluginHierarchyModel::rowCount(const QModelIndex& parent) const
{
    PluginHierarchyItem* parentItem;

    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = _rootItem;
    else
        parentItem = static_cast<PluginHierarchyItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int PluginHierarchyModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return static_cast<PluginHierarchyItem*>(parent.internalPointer())->columnCount();
    return _rootItem->columnCount();
}

Qt::DropActions PluginHierarchyModel::supportedDragActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

PluginHierarchyItem* PluginHierarchyModel::getItem(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return nullptr;

    if (role != Qt::DisplayRole)
        return nullptr;

    PluginHierarchyItem* item = static_cast<PluginHierarchyItem*>(index.internalPointer());

    return item;
}

Qt::ItemFlags PluginHierarchyModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    
    return Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | QAbstractItemModel::flags(index);
}

QVariant PluginHierarchyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return _rootItem->getDataAtColumn(section);

    return QVariant();
}

QMimeData* PluginHierarchyModel::mimeData(const QModelIndexList &indexes) const
{
    QVector<PluginHierarchyItem*> items;

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
