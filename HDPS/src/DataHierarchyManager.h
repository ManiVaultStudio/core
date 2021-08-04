#ifndef HDPS_DATA_HIERARCHY_MANAGER_H
#define HDPS_DATA_HIERARCHY_MANAGER_H

#include "DataHierarchyItem.h"

#include <QObject>
#include <QMap>
#include <QString>
#include <QDebug>

namespace hdps
{

class Core;

class DataHierarchyManager : public QObject
{
    Q_OBJECT

public:
    DataHierarchyManager(QObject* parent = nullptr);

    /**
     * Add a dataset to the hierarchy
     * @param datasetName Name of the dataset
     * @param parentDatasetName Name of the parent dataset (if any)
     * @param visibleInGui Whether the dataset is visible in the gui
     */
    void addDataset(const QString& datasetName, const QString& parentDatasetName = "", const bool& visibleInGui = true);

    /**
     * Removes a dataset from the hierarchy
     * @param datasetName Name of the dataset
     * @return Whether the dataset was removed
     */
    bool removeDataset(const QString& datasetName);

    /**
     * Set the hierarchy item description
     * @param datasetName Name of the dataset
     * @param description Description
     */
    void setItemDescription(const QString& datasetName, const QString& description);

    /**
     * Set the hierarchy item progress
     * @param datasetName Name of the dataset
     * @param progress Progress [0, 1]
     */
    void setItemProgress(const QString& datasetName, const double& progress);

    /**
     * Get hierarchy item by dataset name
     * @param datasetName Name of the dataset
     * @return Data hierarchy item
     */
    const DataHierarchyItem& getHierarchyItem(const QString& datasetName) const;

    /**
     * Get hierarchy item by dataset name
     * @param datasetName Name of the dataset
     * @return Data hierarchy item
     */
    DataHierarchyItem& getHierarchyItem(const QString& datasetName);

protected:

    /**
     * Get dataset children
     * @param datasetName Name of the dataset
     * @param recursive Whether to get all children in a recursive manner
     * @return Children
     */
    QStringList getChildren(const QString& datasetName, const bool& recursive = true);

signals:

    /**
     * Invoked when a hierarchy item is added to the hierarchy manager
     * @param dataHierarchyItem Added item
     */
    void hierarchyItemAdded(DataHierarchyItem& dataHierarchyItem);

    /**
     * Invoked when a hierarchy item is removed from the hierarchy manager
     * @param datasetName Name of the removed dataset
     */
    void hierarchyItemRemoved(DataHierarchyItem& dataHierarchyItem);

private:
    DataHierarchyItemsMap   _dataHierarchyItemsMap;     /** Data hierarchy items map */
};

}

#endif // HDPS_DATA_HIERARCHY_MANAGER_H
