#include "DataHierarchyManager.h"
#include "DataManager.h"

#include <QMessageBox>

#include <stdexcept>

namespace hdps
{

DataHierarchyManager::DataHierarchyManager(QObject* parent /*= nullptr*/) :
    QObject(parent),
    _dataHierarchyItems()
{
}

void DataHierarchyManager::addItem(const QString& datasetName, const QString& parentDatasetName /*= ""*/, const bool& visible /*= true*/)
{
    qDebug() << "Adding" << datasetName << "to the data hierarchy";

    try {
        if (datasetName.isEmpty())
            throw std::runtime_error("Dataset name is empty");

        // Create new data hierarchy item
        const auto newDataHierarchyItem = new DataHierarchyItem(this, datasetName, parentDatasetName, visible);

        _dataHierarchyItems << newDataHierarchyItem;

        if (!parentDatasetName.isEmpty())
            getItem(parentDatasetName)->addChild(datasetName);

        emit itemAdded(newDataHierarchyItem);
    }
    catch (std::exception& e) {
        QMessageBox::critical(nullptr, "Unable to add dataset to data hierarchy", e.what());
    }
}

QStringList DataHierarchyManager::removeItem(const QString& datasetName, const bool& recursively /*= false*/)
{
    qDebug() << "Removing" << datasetName << "from the data hierarchy";

    QStringList datasetsToRemove({ datasetName });

    try {
        if (datasetName.isEmpty())
            throw std::runtime_error("Dataset name is empty");

        const auto hierarchyItem = getItem(datasetName);

        if (hierarchyItem == nullptr)
            throw std::runtime_error(QString("%1 does not exist in the data hierarchy").arg(datasetName).toLatin1());

        if (recursively) {
            auto dataHierarchyItem  = getItem(datasetName);
            auto children           = getChildren(dataHierarchyItem);

            // Build a list of dataset names which need to be removed
            for (auto child : children)
                datasetsToRemove << child->getDatasetName();
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
            emit itemAboutToBeRemoved(datasetToRemove);
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

    return datasetsToRemove;
}

const DataHierarchyItem* DataHierarchyManager::getItem(const QString& datasetName) const
{
    return const_cast<DataHierarchyManager*>(this)->getItem(datasetName);
}

DataHierarchyItem* DataHierarchyManager::getItem(const QString& datasetName)
{
    Q_ASSERT(!datasetName.isEmpty());

    for (auto dataHierarchyItem : _dataHierarchyItems)
        if (dataHierarchyItem->getDatasetName() == datasetName)
            return dataHierarchyItem;

    return nullptr;
}

void DataHierarchyManager::selectItem(const QString& datasetName)
{
    qDebug() << "Selecting" << datasetName << "in the data hierarchy";

    try {
        if (datasetName.isEmpty())
            throw std::runtime_error("Dataset name is empty");

        const auto hierarchyItem = getItem(datasetName);

        if (hierarchyItem == nullptr)
            throw std::runtime_error(QString("%1 does not exist in the data hierarchy").arg(datasetName).toLatin1());

        for (auto dataHierarchyItem : _dataHierarchyItems)
            if (dataHierarchyItem->isSelected())
                dataHierarchyItem->deselect();

        hierarchyItem->select();
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
