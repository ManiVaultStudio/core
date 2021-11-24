#pragma once

#include "DataHierarchyItem.h"

#include <QObject>
#include <QMap>
#include <QString>
#include <QDebug>

namespace hdps
{

class DataManager;

class DataHierarchyManager : public QObject
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    DataHierarchyManager(QObject* parent = nullptr);

    /**
     * Add a dataset to the hierarchy
     * @param dataset Smart pointer to dataset
     * @param parentDataset Smart pointer to parent dataset (if any)
     * @param visible Whether the dataset is visible in the gui
     */
    void addItem(Dataset<DatasetImpl>& dataset, Dataset<DatasetImpl>& parentDataset, const bool& visible = true);

    /**
     * Removes an item from the hierarchy
     * @param dataset Smart pointer to the dataset
     * @return vector of pointers to datasets that need to removed
     */
    QVector<Dataset<DatasetImpl>> removeItem(const Dataset<DatasetImpl>& dataset, const bool& recursively = false);

    /**
     * Get hierarchy item by dataset globally unique identifier
     * @param datasetGuid Dataset GUID
     * @return Reference to data hierarchy item
     */
    const DataHierarchyItem& getItem(const QString& datasetGuid) const;

    /**
     * Get hierarchy item by dataset globally unique identifier
     * @param datasetGuid Dataset GUID
     * @return Reference to data hierarchy item
     */
    DataHierarchyItem& getItem(const QString& datasetGuid);

    /**
     * Selects data hierarchy item with dataset name
     * @param dataSet Smart pointer to dataset to select
     */
    void selectItem(Dataset<DatasetImpl>& dataSet);

    /**
     * Get dataset children
     * @param dataHierarchyItem Pointer to data hierarchy item
     * @param recursive Whether to get all children in a recursive manner
     * @return Children
     */
    DataHierarchyItems getChildren(DataHierarchyItem* dataHierarchyItem, const bool& recursive = true);

signals:

    /**
     * Signals that a hierarchy item is added to the hierarchy manager
     * @param dataHierarchyItem Reference to added data hierarchy item
     */
    void itemAdded(DataHierarchyItem& dataHierarchyItem);

    /**
     * Signals that a hierarchy item is about to be removed from the hierarchy manager
     * @param dataSet Smart pointer to the about to be removed dataset
     */
    void itemAboutToBeRemoved(const Dataset<DatasetImpl>& dataSet);

    /**
     * Signals that a hierarchy item is removed from the hierarchy manager
     * @param datasetGui GUID of the removed dataset
     */
    void itemRemoved(const QString& datasetGui);

    /**
     * Signals that a hierarchy item has been re-located
     * @param itemToBeRelocated Reference to data hierarchy item that will be relocated
     */
    void itemAboutToBeRelocated(DataHierarchyItem& itemToBeRelocated);

    /**
     * Signals that a hierarchy item has been relocated
     * @param relocatedItem Reference to relocated data hierarchy item
     */
    void itemRelocated(DataHierarchyItem& relocatedItem);

private:
    DataHierarchyItems    _dataHierarchyItems;    /** Shared pointers to data hierarchy items */
};

}
