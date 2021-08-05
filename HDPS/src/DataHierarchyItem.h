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
     * @param visible Whether the dataset is visible
     * @param selected Whether the dataset is selected
     */
    DataHierarchyItem(QObject* parent = nullptr, const QString& datasetName = "", const QString& parentDatasetName = "", const bool& visible = true, const bool& selected = false);

    /** Gets the dataset name */
    QString getDatasetName() const;

    /** Gets the parent dataset name */
    QString getParent() const;

    /** Gets the names of the children name */
    QStringList getChildren() const;

    /** Gets the number of children */
    std::uint32_t getNumberOfChildren() const;

    /** Gets whether the dataset is visible */
    bool getVisible() const;

    /** Gets whether the dataset is hidden */
    bool isHidden() const;

    /** Gets the description */
    QString getDescription() const;

    /**
     * Sets the description
     * @param description Description
     */
    void setDescription(const QString& description);

    /** Gets the item progress */
    float getProgress() const;

    /**
     * Sets the item progress
     * @param progress Item progress
     */
    void setProgress(const float& progress);

    /** Gets whether the hierarchy item is selected */
    bool isSelected() const;

    /**
     * Sets the item selection status
     * @param selected Whether the hierarchy item is selected
     */
    void setSelected(const bool& selected);

    /** Selects the hierarchy item */
    void select();

    /** De-selects the hierarchy item */
    void deselect();

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
        _visible        = other._visible;
        _description    = other._description;
        _progress       = other._progress;
        _selected       = other._selected;

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
    void progressChanged(const float& progress);

    /**
     * Signals that the item got selected
     * @param selected Whether the item is selected
     */
    void selectionChanged(const bool& selected);

protected:
        
    QString         _datasetName;       /** Name of the dataset */
    QString         _parent;            /** Parent item */
    QStringList     _children;          /** Child items (if any) */
    bool            _visible;           /** Whether the dataset is visible */
    QString         _description;       /** Description */
    float           _progress;          /** Progress */
    bool            _selected;          /** Whether the hierarchy item is selected */

public:
    static Core* core;  /** Pointer to core */
};

/** Shared pointer of data hierarchy item */
using SharedDataHierarchyItem = QSharedPointer<DataHierarchyItem>;

/** Maps string to shared data hierarchy item */
using DataHierarchyItemsMap = QMap<QString, SharedDataHierarchyItem>;

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
