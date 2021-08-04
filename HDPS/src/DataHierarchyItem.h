#ifndef HDPS_DATA_HIERARCHY_ITEM_H
#define HDPS_DATA_HIERARCHY_ITEM_H

#include "DataType.h"

#include <QObject>
#include <QMap>
#include <QString>
#include <QDebug>

namespace hdps
{

class DataSet;
class Core;

 /**
  * Data hierarchy item class
  *
  * Represents a dataset as an item in a dataset hierarchy
  *
  * @author Thomas Kroes
  */
class DataHierarchyItem : public QObject
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param datasetName Name of the dataset
     * @param parentDatasetName Name of the parent dataset
     * @param visibleInGui Whether the dataset is visible in the GUI
     */
    DataHierarchyItem(QObject* parent = nullptr, const QString& datasetName = "", const QString& parentDatasetName = "", const bool& visibleInGui = true);

    /** Gets the dataset name */
    QString getDatasetName() const;

    /** Gets the parent dataset name */
    QString getParent() const;

    /** Gets the names of the children name */
    QStringList getChildren() const;

    /** Gets the number of children */
    std::uint32_t getNumberOfChildren() const;

    /** Gets whether the dataset is visible in the GUI */
    QString getVisibleInGui() const;

    /**
     * Gets/sets the description
     * @param description Description
     */
    QString getDescription() const;
    void setDescription(const QString& description);

    /**
     * Gets/sets the progress
     * @param progress Progress
     */
    float getProgress() const;
    void setProgress(const float& progress);

    /**
     * Adds a child (name reference to data hierarchy item)
     * @param name Name of the child
     */
    void addChild(const QString& name);

    /**
     * Removes a child (name reference to data hierarchy item)
     * @param name Name of the child
     */
    void removeChild(const QString& name);

    /** Gets the string representation of the hierarchy item */
    QString toString() const;

    /** Get the dataset */
    DataSet& getDataset() const;

    /** Get the dataset type */
    DataType getDataType() const;

    /**
     * Assignment operator
     * @param other Data hierarchy item to copy from
     */
    DataHierarchyItem& operator= (const DataHierarchyItem& other)
    {
        _datasetName    = other._datasetName;
        _parent         = other._parent;
        _children       = other._children;
        _visibleInGui   = other._visibleInGui;
        _description    = other._description;
        _progress       = other._progress;

        return *this;
    }

signals:

    /**
     * Signals that the description changed
     * @param description Description
     */
    void descriptionChanged(const QString& description);

    /**
     * Signals that the progress changed
     * @param progress Progress
     */
    void progressChanged(const double& progress);

protected:
        
    QString         _datasetName;       /** Name of the dataset */
    QString         _parent;            /** Parent item */
    QStringList     _children;          /** Child items (if any) */
    bool            _visibleInGui;      /** Whether the dataset is visible in the GUI */
    QString         _description;       /** Description */
    float           _progress;          /** Progress */

public:
    static Core* core;  /** Pointer to core */
};

/** Maps string to data hierarchy item */
using DataHierarchyItemsMap = QMap<QString, DataHierarchyItem*>;

/**
 * Print to console
 * @param debug Debug
 * @param dataHierarchyItem Data hierarchy item
 */
inline QDebug operator<<(QDebug debug, const DataHierarchyItem& dataHierarchyItem)
{
    debug.nospace() << dataHierarchyItem.toString();

    return debug.space();
}

}

#endif // HDPS_DATA_HIERARCHY_ITEM_H
