#include "DataHierarchyManager.h"
#include "DataManager.h"

#include "util/Exception.h"

#include <QMessageBox>

#include <stdexcept>

using namespace hdps::util;

namespace hdps
{

DataHierarchyManager::DataHierarchyManager(QObject* parent /*= nullptr*/) :
    QObject(parent),
    _dataHierarchyItems()
{
}

void DataHierarchyManager::addItem(Dataset<DatasetImpl>& dataset, Dataset<DatasetImpl>& parentDataset, const bool& visible /*= true*/)
{
    qDebug() << "Adding" << dataset->getGuiName() << "to the data hierarchy";

    try {

        // Create new data hierarchy item
        const auto newDataHierarchyItem = new DataHierarchyItem(this, dataset, parentDataset, visible);

        _dataHierarchyItems << newDataHierarchyItem;

        // Add child item if the parent is valid
        if (parentDataset.isValid())
            parentDataset->getDataHierarchyItem().addChild(dataset->getDataHierarchyItem());

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
            emit itemAboutToBeRemoved(*datasetToRemove);
            {
                // Get reference to data hierarchy item
                auto& dataHierarchyItemToRemove = datasetToRemove->getDataHierarchyItem();

                // Remove from internal list
                _dataHierarchyItems.removeOne(&dataHierarchyItemToRemove);

                // Free memory
                delete &dataHierarchyItemToRemove;
            }
            emit itemRemoved(datasetToRemove->getGuid());
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

}
