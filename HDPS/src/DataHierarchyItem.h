#ifndef HDPS_DATA_HIERARCHY_ITEM_H
#define HDPS_DATA_HIERARCHY_ITEM_H

#include "Core.h"
#include "Set.h"

#include <QObject>
#include <QVector>

class QMenu;

namespace hdps
{

class DataHierarchyItem : public QObject
{
public:
    using PluginHierarchyItems = QVector<DataHierarchyItem*>;

public:
    /**
     * Constructor
     * @param datasetName Name of the dataset (empty for root)
     * @param parent Parent (if any)
     */
    explicit DataHierarchyItem(const QString& datasetName = "", DataHierarchyItem* parent = nullptr);

    /** Destructor */
    virtual ~DataHierarchyItem();

    /**
     * Add a child item
     * @param item Child item to add
     */
    void addChild(DataHierarchyItem* item);
    
    /** Get/set parent item */
    DataHierarchyItem* getParent();
    void setParent(DataHierarchyItem* parent);

    /**
     * Get child at row
     * @param row Row index of the child
     * @return Child item
     */
    DataHierarchyItem* getChild(const std::int32_t& row);

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
     * @return Data in string format
     */
    QString getDataAtColumn(const std::uint32_t& column) const;

    /** Returns the item icon */
    QIcon getIcon();

    /** Returns the item context menu */
    QMenu* getContextMenu();

    /** Returns the name of the dataset (empty of root item) */
    QString getDatasetName() const;

protected:
    DataHierarchyItem*      _parent;            /** Pointer to parent item */   
    PluginHierarchyItems    _children;          /** Pointers to child items */
    QString                 _datasetName;       /** Name of the dataset */
    DataSet*                _dataset;           /** Pointer to the dataset */

public:
    static Core* core;  /** Static pointer to the core */
};

}

#endif // HDPS_DATA_HIERARCHY_ITEM_H
