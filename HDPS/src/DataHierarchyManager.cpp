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

        if (parentDataset.isValid())
            parentDataset->getDataHierarchyItem().addChild(dataset->getDataHierarchyItem());

        emit itemAdded(newDataHierarchyItem);
    }
    catch (std::exception& e) {
        QMessageBox::critical(nullptr, "Unable to add dataset to data hierarchy", e.what());
    }
}

QVector<Dataset<DatasetImpl>> DataHierarchyManager::removeItem(const Dataset<DatasetImpl>& dataset, const bool& recursively /*= false*/)
{
    qDebug() << "Removing" << dataset->getGuiName() << "from the data hierarchy";

    QVector<Dataset<DatasetImpl>> datasetsToRemove({ dataset });

    /*
    try {
        const auto hierarchyItem = getItem(datasetName);

        if (hierarchyItem == nullptr)
            throw std::runtime_error(QString("%1 does not exist in the data hierarchy").arg(datasetName).toLatin1());

        if (recursively) {
            auto dataHierarchyItem  = getItem(datasetName);
            auto children           = getChildren(dataHierarchyItem);

            // Build a list of dataset names which need to be removed
            for (auto child : children)
                datasetsToRemove << &child->getDataset();
        }
        else {

            // Get the immediate children which need to be re-located
            const auto immediateChildren = getChildren(getItem(datasetName), false);

            // Relocate the immediate children to the root
            for (auto immediateChild : immediateChildren) {
                emit itemAboutToBeRelocated(immediateChild);
                {
                    immediateChild->setParent(nullptr);
                }
                emit itemRelocated(immediateChild);
            }
        }

        // Reverse the dataset names so we remove bottom-up
        std::reverse(datasetsToRemove.begin(), datasetsToRemove.end());

        // Remove the items from the list
        for (auto datasetToRemove : datasetsToRemove) {
            emit itemAboutToBeRemoved(*datasetToRemove);
            {
                const auto itemToRemove = getItem(datasetToRemove);

                // Remove from internal list
                _dataHierarchyItems.removeOne(itemToRemove);

                // Free memory
                delete itemToRemove;
            }
            emit itemRemoved(datasetToRemove);
        }
    } catch (std::exception& e) {
        QMessageBox::critical(nullptr, "Unable to remove dataset(s) from data hierarchy", e.what());
    }
    */
    return datasetsToRemove;
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
        exceptionMessageBox("Unable to retrieve data hierarchy item", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to retrieve data hierarchy item");
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
    catch (std::exception& e) {
        QMessageBox::critical(nullptr, "Unable to select dataset in data hierarchy", e.what());
    }
}

DataHierarchyItems DataHierarchyManager::getChildren(DataHierarchyItem* dataHierarchyItem, const bool& recursive /*= true*/)
{
    Q_ASSERT(dataHierarchyItem != nullptr);

    auto children = dataHierarchyItem->getChildren();

    if (recursive)
        for (auto child : children)
            children << getChildren(child, recursive);

    return children;
}

}
