#pragma once

#include "DataHierarchyManager.h"

#include <QObject>
#include <QVector>
#include <QRandomGenerator>

class QMenu;

namespace hdps
{

class DataHierarchyModelItem : public QObject
{
Q_OBJECT

public:
    using PluginHierarchyItems = QVector<DataHierarchyModelItem*>;

    /** Columns */
    enum Column {
        Name,           /** Name of the dataset */
        GUID,           /** Globally unique dataset identifier */
        Info,           /** Info (e.g. to report progress) */
        Progress,       /** Task progress in percentage */
        GroupIndex,     /** Dataset group index */
        IsGroup,        /** Whether the dataset is composed of other datasets */
        IsAnalyzing,    /** Whether an analysis is taking place on the dataset */
        IsLocked,       /** Whether the dataset is locked */

        _start = Name,
        _end = IsLocked
    };

public:

    /**
     * Constructor
     * @param dataHierarchyItem Pointer to data hierarchy item
     * @param parent Parent (if any)
     */
    explicit DataHierarchyModelItem(DataHierarchyItem* dataHierarchyItem, DataHierarchyModelItem* parent = nullptr);

    /** Destructor */
    virtual ~DataHierarchyModelItem();

    /**
     * Add a child item
     * @param item Child item to add
     */
    void addChild(DataHierarchyModelItem* item);
    
    /** Get/set parent item */
    DataHierarchyModelItem* getParent();
    void setParent(DataHierarchyModelItem* parent);

    /**
     * Get child at row
     * @param row Row index of the child
     * @return Child item
     */
    DataHierarchyModelItem* getChild(const std::int32_t& row);

    /** Returns the child index w.r.t. the parent */
    std::int32_t row() const;

    /** Returns the number of children */
    std::int32_t getNumChildren() const;

    /** Returns the number of columns */
    std::int32_t getNumColumns() const;

    /** Returns the serialized item string */
    QString serialize() const;

    /**
     * Get data at column
     * @param column Column index
     * @param role Data role
     * @return Data in string format
     */
    QVariant getDataAtColumn(const std::uint32_t& column, int role = Qt::DisplayRole) const;

    /**
     * Renames the dataset
     * @param intendedDatasetName Intended new name of the dataset
     */
    void renameDataset(const QString& intendedDatasetName);

    /**
     * Set group index
     * @param groupIndex group index
     */
    void setGroupIndex(const std::int32_t& groupIndex);

    /**
     * Get corresponding data hierarchy item
     * @return Pointer to data hierarchy item
     */
    DataHierarchyItem* getDataHierarchyItem();

    /** Get whether the item is visible or not */
    bool isVisible() const;

protected:

    /**
     * Removes a child (name reference to data hierarchy item)
     * @param dataHierarchyModelItem Pointer to data hierarchy model item
     */
    void removeChild(DataHierarchyModelItem* dataHierarchyModelItem);

protected:
    DataHierarchyModelItem* _parent;                /** Pointer to parent item */
    PluginHierarchyItems    _children;              /** Pointers to child items */
    DataHierarchyItem*      _dataHierarchyItem;     /** Pointer to data hierarchy item*/

    /** Random number generator for pseudo-random group index colors */
    static QRandomGenerator rng;
};

}
