#ifndef HDPS_DATA_HIERARCHY_MODEL_ITEM_H
#define HDPS_DATA_HIERARCHY_MODEL_ITEM_H

#include "Core.h"
#include "DataHierarchyManager.h"

#include <QObject>
#include <QVector>

class QMenu;

namespace hdps
{

class DataHierarchyModelItem : public QObject
{
Q_OBJECT

public:
    using PluginHierarchyItems = QVector<DataHierarchyModelItem*>;

    /** Columns */
    enum class Column {
        Name,
        Description,
        Analysis,
        Progress,
        Analyzing,

        _start = Name,
        _end = Analyzing
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
     * @return Data in string format
     */
    QString getDataAtColumn(const std::uint32_t& column) const;

    /**
     * Get icon at column
     * @param column Column index
     * @return Icon
     */
    QIcon getIconAtColumn(const std::uint32_t& column) const;

    /** Returns the item context menu */
    QMenu* getContextMenu();

    /**
     * Renames the dataset
     * @param intendedDatasetName Intended new name of the dataset
     */
    void renameDataset(const QString& intendedDatasetName);

protected:

    /**
     * Removes a child (name reference to data hierarchy item)
     * @param dataHierarchyModelItem Pointer to data hierarchy model item
     */
    void removeChild(DataHierarchyModelItem* dataHierarchyModelItem);

public: // Analysis

    /**
     * Set analysis progress percentage
     * @param progressPercentage Progress percentage of the analysis
     */
    void setProgressPercentage(const float& progressPercentage);

    /**
     * Set analysis progress section
     * @param progressSection Progress section of the analysis
     */
    void setProgressSection(const QString& progressSection);

protected:
    DataHierarchyModelItem*     _parent;                    /** Pointer to parent item */
    PluginHierarchyItems        _children;                  /** Pointers to child items */
    DataHierarchyItem*          _dataHierarchyItem;         /** Pointer to data hierarchy item*/
    QString                     _progressSection;           /** Progress section of the analysis */
    float                       _progressPercentage;        /** Progress percentage of the analysis */

public:
    static Core* core;  /** Static pointer to the core */
};

}

#endif // HDPS_DATA_HIERARCHY_MODEL_ITEM_H
