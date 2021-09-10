#ifndef HDPS_DATA_HIERARCHY_MANAGER_H
#define HDPS_DATA_HIERARCHY_MANAGER_H

#include "DataHierarchyItem.h"

#include <QObject>
#include <QMap>
#include <QString>
#include <QDebug>

namespace hdps
{

class DataHierarchyManager : public QObject
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param dataManager Reference to data manager
     */
    DataHierarchyManager(DataManager& dataManager, QObject* parent = nullptr);

    /**
     * Add a dataset to the hierarchy
     * @param datasetName Name of the dataset
     * @param parentDatasetName Name of the parent dataset (if any)
     * @param visible Whether the dataset is visible in the gui
     */
    void addDataset(const QString& datasetName, const QString& parentDatasetName = "", const bool& visible = true);

    /**
     * Removes a dataset from the hierarchy
     * @param datasetName Name of the dataset
     * @return Whether the dataset was removed
     */
    bool removeDataset(const QString& datasetName, const bool& recursively = false);

    /**
     * Get hierarchy item by dataset name
     * @param datasetName Name of the dataset
     * @return Pointer to data hierarchy item
     */
    const DataHierarchyItem* getHierarchyItem(const QString& datasetName) const;

    /**
     * Get hierarchy item by dataset name
     * @param datasetName Name of the dataset
     * @return Pointer to data hierarchy item
     */
    DataHierarchyItem* getHierarchyItem(const QString& datasetName);

    /**
     * Selects data hierarchy item with dataset name
     * @param datasetName Name of the dataset to select
     */
    void selectHierarchyItem(const QString& datasetName);

    /**
     * Get dataset children
     * @param dataHierarchyItem Pointer to data hierarchy item
     * @param recursive Whether to get all children in a recursive manner
     * @return Children
     */
    DataHierarchyItems getChildren(DataHierarchyItem* dataHierarchyItem, const bool& recursive = true);

protected:

    /**
     * Remove a child by dataset name
     * @param datasetName Name of the dataset
     * @param recursively Recursively remove all children
     */
    void removeDataHierarchyItemByDatasetName(const QString& datasetName, const bool& recursively = false);

signals:

    /**
     * Signals that a hierarchy item is added to the hierarchy manager
     * @param dataHierarchyItem Pointer to added data hierarchy item
     */
    void itemAdded(DataHierarchyItem* dataHierarchyItem);

    /**
     * Signals that a hierarchy item is about to be removed from the hierarchy manager
     * @param datasetName Name of the about to be removed dataset
     */
    void itemAboutToBeRemoved(const QString& datasetName);

    /**
     * Signals that a hierarchy item is removed from the hierarchy manager
     * @param datasetName Name of the removed dataset
     */
    void itemRemoved(const QString& datasetName);

    /**
     * Signals that a hierarchy item has been re-located
     * @param relocatedItem Pointer to data hierarchy item that will be relocated
     */
    void itemAboutToBeRelocated(DataHierarchyItem* itemToBeRelocated);

    /**
     * Signals that a hierarchy item has been relocated
     * @param relocatedItem Pointer to relocated data hierarchy item
     */
    void itemRelocated(DataHierarchyItem* relocatedItem);

private:
    DataManager&                _dataManager;           /** Reference to data manager */
    SharedDataHierarchyItems    _dataHierarchyItems;    /** Shared pointers to data hierarchy items */
};

}

#endif // HDPS_DATA_HIERARCHY_MANAGER_H
