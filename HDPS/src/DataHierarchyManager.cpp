#include "DataHierarchyManager.h"
#include "DataManager.h"

#include "util/Exception.h"

#include <QMessageBox>

#include <stdexcept>

using namespace hdps::util;

namespace hdps
{

DataHierarchyManager::DataHierarchyManager(QObject* parent /*= nullptr*/) :
    WidgetAction(parent),
    _dataHierarchyItems()
{
    setText("Data hierarchy");
    setObjectName("Hierarchy");
}

void DataHierarchyManager::addItem(Dataset<DatasetImpl>& dataset, Dataset<DatasetImpl>& parentDataset, const bool& visible /*= true*/)
{
    qDebug() << "Adding" << dataset->getGuiName() << "to the data hierarchy";

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

        _dataHierarchyItems << newDataHierarchyItem;

        // Add child item if the parent is valid
        if (parentDataset.isValid())
            parentDataset->getDataHierarchyItem().addChild(dataset->getDataHierarchyItem());

        dataset->setParent(newDataHierarchyItem);

        // Notify others that an item is added
        emit itemAdded(*newDataHierarchyItem);

        // Notify others that a child is added
        if (parentDataset.isValid())
            Application::core()->notifyDataChildAdded(parentDataset, dataset);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to add dataset to the data hierarchy manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to add dataset to the data hierarchy manager");
    }
}

void DataHierarchyManager::removeItem(const Dataset<DatasetImpl>& dataset, const bool& recursively /*= false*/)
{
    try {
        qDebug() << "Removing" << dataset->getGuiName() << "from the data hierarchy";

        QVector<Dataset<DatasetImpl>> datasetsToRemove({ dataset });

        // Add children recursively
        for (auto dataHierarchyItem : const_cast<Dataset<DatasetImpl>&>(dataset)->getDataHierarchyItem().getChildren(true))
            datasetsToRemove << dataHierarchyItem->getDataset();

        // Reverse the dataset names so we remove bottom-up
        std::reverse(datasetsToRemove.begin(), datasetsToRemove.end());

        // Remove the items from the list
        for (auto& datasetToRemove : datasetsToRemove) {
            
            // Get reference to data hierarchy item
            auto& dataHierarchyItemToRemove = datasetToRemove->getDataHierarchyItem();

            // Get smart pointer to parent dataset
            const auto parentDataset = dataHierarchyItemToRemove.getParent().getDataset();

            // Get GUID of the the dataset which is about to be removed
            const auto datasetGuid = dataHierarchyItemToRemove.getDataset()->getGuid();

            emit itemAboutToBeRemoved(*datasetToRemove);
            {
                // Remove from internal list
                _dataHierarchyItems.removeOne(&dataHierarchyItemToRemove);

                // Free memory
                delete &dataHierarchyItemToRemove;
            }
            emit itemRemoved(datasetToRemove->getGuid());

            // Notify others that a child is removed
            if (dataset->getDataHierarchyItem().hasParent())
                Application::core()->notifyDataChildRemoved(parentDataset, datasetGuid);
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to remove item(s) from the data hierarchy manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to remove item(s) from the data hierarchy manager");
    }
}

const DataHierarchyItem& DataHierarchyManager::getItem(const QString& datasetGuid) const
{
    return const_cast<DataHierarchyManager*>(this)->getItem(datasetGuid);
}

DataHierarchyItem& DataHierarchyManager::getItem(const QString& datasetGuid)
{
    try
    {
        Q_ASSERT(!datasetGuid.isEmpty());

        for (auto dataHierarchyItem : _dataHierarchyItems)
            if (dataHierarchyItem->getDataset().getDatasetGuid() == datasetGuid)
                return *dataHierarchyItem;
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get item from the data hierarchy manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to get item from the data hierarchy manager");
    }
}

void DataHierarchyManager::selectItem(Dataset<DatasetImpl>& dataSet)
{
    qDebug() << "Selecting" << dataSet->getGuiName() << "in the data hierarchy";

    try {

        // Get data hierarchy item that needs to be selected
        auto& hierarchyItem = dataSet->getDataHierarchyItem();

        // Deselect other data hierarchy items
        for (auto dataHierarchyItem : _dataHierarchyItems)
            if (dataHierarchyItem->isSelected())
                dataHierarchyItem->deselect();

        // Select the data hierarchy item
        hierarchyItem.select();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to select item in the data hierarchy manager", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to select item in the data hierarchy manager");
    }
}

DataHierarchyItems DataHierarchyManager::getChildren(DataHierarchyItem* dataHierarchyItem, const bool& recursive /*= true*/)
{
    Q_ASSERT(dataHierarchyItem != nullptr);

    auto children = dataHierarchyItem->getChildren();

    // Get children recursively
    if (recursive)
        for (auto child : children)
            children << getChildren(child, recursive);

    return children;
}

void DataHierarchyManager::fromVariantMap(const QVariantMap& variantMap)
{
    // Load dataset from variant and add to the data hierarchy manager
    const auto loadDataset = [](const QVariantMap& dataHierarchyItemMap, const QString& guiName, Dataset<DatasetImpl> parent) -> Dataset<DatasetImpl> {
        const auto dataset      = dataHierarchyItemMap["Dataset"].toMap();
        const auto pluginKind   = dataset["PluginKind"].toString();
        const auto children     = dataset["Children"].toMap();

        // Add dataset to the data hierarchy manager
        auto loadedDataset = Application::core()->addDataset(pluginKind, guiName, parent);

        // Load the data hierarchy item
        loadedDataset->getDataHierarchyItem().fromVariantMap(dataHierarchyItemMap);

        // And load from variant map
        loadedDataset->fromVariantMap(dataset);

        return loadedDataset;
    };

    const std::function<void(const QVariantMap&, Dataset<DatasetImpl>)> loadDataHierarchyItem = [&loadDataHierarchyItem, loadDataset](const QVariantMap& variantMap, Dataset<DatasetImpl> parent) -> void {

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
    if (_dataHierarchyItems.isEmpty())
        return QVariantMap();

    QVariantMap variantMap;

    // Root items sort index
    std::uint32_t sortIndex = 0;

    // Export root data hierarchy items
    for (auto dataHierarchyItem : _dataHierarchyItems) {

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
