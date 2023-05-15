#include "DataHierarchyManager.h"
#include "DataManager.h"

#include <util/Exception.h>
#include <Application.h>

#include <QMessageBox>

#include <stdexcept>

using namespace hdps::util;

#ifdef _DEBUG
    #define DATA_HIERARCHY_MANAGER_VERBOSE
#endif

namespace hdps
{

DataHierarchyManager::DataHierarchyManager(QObject* parent /*= nullptr*/) :
    AbstractDataHierarchyManager(),
    _items()
{
    setObjectName("Hierarchy");
}

DataHierarchyManager::~DataHierarchyManager()
{
    reset();
}

void DataHierarchyManager::initialize()
{
#ifdef DATA_HIERARCHY_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    AbstractManager::initialize();
}

void DataHierarchyManager::reset()
{
#ifdef DATA_HIERARCHY_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    beginReset();
    {
    }
    endReset();
}

void DataHierarchyManager::addItem(Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, const bool& visible /*= true*/)
{
    //qDebug() << "Adding" << dataset->getGuiName() << "to the data hierarchy";

    try {

        // Create new data hierarchy item
        const auto newDataHierarchyItem = new DataHierarchyItem(parentDataset.isValid() ? &parentDataset->getDataHierarchyItem() : static_cast<QObject*>(this), dataset, parentDataset, visible);

        // Pass-through loading signal
        connect(newDataHierarchyItem, &DataHierarchyItem::loading, this, [this, newDataHierarchyItem]() {
            emit itemLoading(*newDataHierarchyItem);
        });

        // Pass-through loaded signal
        connect(newDataHierarchyItem, &DataHierarchyItem::loaded, this, [this, newDataHierarchyItem]() {
            emit itemLoaded(*newDataHierarchyItem);
        });

        // Pass-through saving signal
        connect(newDataHierarchyItem, &DataHierarchyItem::saving, this, [this, newDataHierarchyItem]() {
            emit itemSaving(*newDataHierarchyItem);
        });

        // Pass-through saved signal
        connect(newDataHierarchyItem, &DataHierarchyItem::saved, this, [this, newDataHierarchyItem]() {
            emit itemSaved(*newDataHierarchyItem);
        });

        _items << newDataHierarchyItem;

        // Add child item if the parent is valid
        if (parentDataset.isValid())
            parentDataset->getDataHierarchyItem().addChild(dataset->getDataHierarchyItem());

        dataset->setParent(newDataHierarchyItem);

        // Notify others that an item is added
        emit itemAdded(*newDataHierarchyItem);

        // Remove the data hierarchy item when the corresponding dataset is about to be removed
        connect(&newDataHierarchyItem->getDatasetReference(), &Dataset<DatasetImpl>::dataAboutToBeRemoved, this, [this, newDataHierarchyItem]() {
            if (newDataHierarchyItem->getDatasetReference().isValid())
                removeItem(*newDataHierarchyItem);
        });
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to add dataset to the data hierarchy manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to add dataset to the data hierarchy manager");
    }
}

void DataHierarchyManager::removeItem(DataHierarchyItem& dataHierarchyItem)
{
    try {
        qDebug() << "Removing" << dataHierarchyItem.getDataset()->getGuiName() << "from the data hierarchy";

        // Get dataset
        auto dataset        = dataHierarchyItem.getDataset();
        auto datasetGuid    = dataset->getGuid();

        emit itemAboutToBeRemoved(dataset);
        {
            _items.removeOne(&dataHierarchyItem);

            //delete &dataHierarchyItem;
        }
        emit itemRemoved(datasetGuid);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to remove item(s) from the data hierarchy manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to remove item(s) from the data hierarchy manager");
    }
}

void DataHierarchyManager::removeAllItems()
{
    const auto dataHierarchyItems = _items;

    for (auto dataHierarchyItem : dataHierarchyItems)
        if (!dataHierarchyItem->hasParent())
            removeItem(*dataHierarchyItem);
}

const DataHierarchyItem& DataHierarchyManager::getItem(const QString& datasetGuid) const
{
    return const_cast<DataHierarchyManager*>(this)->getItem(datasetGuid);
}

DataHierarchyItem& DataHierarchyManager::getItem(const QString& datasetGuid)
{
    DataHierarchyItem* item = nullptr;

    try
    {
        Q_ASSERT(!datasetGuid.isEmpty());

        for (auto dataHierarchyItem : _items)
            if (dataHierarchyItem->getDatasetReference().getDatasetGuid() == datasetGuid)
                return *dataHierarchyItem;

        QString errorMessage = QString("Failed to find data hierarchy item with guid: %1").arg(datasetGuid);
        throw std::runtime_error(errorMessage.toStdString());
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get item from the data hierarchy manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to get item from the data hierarchy manager");
    }

    return *item;
}

hdps::DataHierarchyItems DataHierarchyManager::getChildren(DataHierarchyItem& dataHierarchyItem, const bool& recursive /*= true*/)
{
    auto children = dataHierarchyItem.getChildren();

    if (recursive)
        for (auto child : children)
            children << getChildren(*child, recursive);

    return children;
}

DataHierarchyItems DataHierarchyManager::getTopLevelItems()
{
    DataHierarchyItems topLevelItems;

    for (auto item : _items)
        if (!item->hasParent())
            topLevelItems << item;

    return topLevelItems;
}

// TODO
void DataHierarchyManager::selectItems(DataHierarchyItems& selectedItems)
{
    emit selectedItemsChanged(selectedItems);
}

void DataHierarchyManager::fromVariantMap(const QVariantMap& variantMap)
{
    // Load dataset from variant and add to the data hierarchy manager
    const auto loadDataset = [](const QVariantMap& dataHierarchyItemMap, const QString& guiName, Dataset<DatasetImpl> parent) -> Dataset<DatasetImpl> {
        const auto dataset      = dataHierarchyItemMap["Dataset"].toMap();
        const auto pluginKind   = dataset["PluginKind"].toString();
        const auto children     = dataset["Children"].toMap();

        auto loadedDataset = Application::core()->addDataset(pluginKind, guiName, parent, dataset["GUID"].toString());

        loadedDataset->getDataHierarchyItem().fromVariantMap(dataHierarchyItemMap);
        loadedDataset->fromVariantMap(dataset);

        events().notifyDatasetAdded(loadedDataset);

        return loadedDataset;
    };

    const std::function<void(const QVariantMap&, Dataset<DatasetImpl>)> loadDataHierarchyItem = [&loadDataHierarchyItem, loadDataset](const QVariantMap& variantMap, Dataset<DatasetImpl> parent) -> void {

        if (Application::isSerializationAborted())
            return;

        // Sorted data hierarchy items (according to their sort index)
        QVector<QVariantMap> sortedItems;

        // Allocate items
        sortedItems.resize(variantMap.count());

        // Sort the items
        for (auto variant : variantMap.values())
            sortedItems[variant.toMap()["SortIndex"].toInt()] = variant.toMap();

        // Add items in intended order
        for (auto item : sortedItems)
            loadDataHierarchyItem(item["Children"].toMap(), loadDataset(item, item["Name"].toString(), parent));
    };

    loadDataHierarchyItem(variantMap, Dataset<DatasetImpl>());
}

QVariantMap DataHierarchyManager::toVariantMap() const
{
    if (_items.isEmpty())
        return QVariantMap();

    QVariantMap variantMap;

    // Root items sort index
    std::uint32_t sortIndex = 0;

    // Export root data hierarchy items
    for (auto dataHierarchyItem : _items) {

        // Only process root data hierarchy item
        if (dataHierarchyItem->hasParent())
            continue;

        // Get map of data hierarchy item
        auto dataHierarchyItemMap = dataHierarchyItem->toVariantMap();

        // Add sort index
        dataHierarchyItemMap["SortIndex"] = sortIndex;

        // Assign data hierarchy item map
        variantMap[dataHierarchyItem->getDataset()->getGuid()] = dataHierarchyItemMap;

        sortIndex++;
    }

    return variantMap;
}

}
