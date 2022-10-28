#pragma once

#include "Core.h"

#include <QObject>
#include <QVector>

class QMenu;

namespace hdps
{

class ActionHierarchyModelItem : public QObject
{
Q_OBJECT
    
    using ActionHierarchyModelItems = QVector<ActionHierarchyModelItem*>;

public:

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
     * @param parent Parent action hierarchy item (if any)
     */
    explicit ActionHierarchyModelItem(ActionHierarchyModelItem* parent = nullptr);

    /** Destructor */
    virtual ~ActionHierarchyModelItem();

    /**
     * Add a child item
     * @param item Child item to add
     */
    void addChild(ActionHierarchyModelItem* item);
    
    /** Get/set parent item */
    ActionHierarchyModelItem* getParent();
    void setParent(ActionHierarchyModelItem* parent);

    /**
     * Get child at row
     * @param row Row index of the child
     * @return Child item
     */
    ActionHierarchyModelItem* getChild(const std::int32_t& row);

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

protected:

    /**
     * Removes child
     * @param actionHierarchyModelItem Pointer to action hierarchy model item to remove
     */
    void removeChild(ActionHierarchyModelItem* actionHierarchyModelItem);

protected:
    ActionHierarchyModelItem*   _parent;        /** Pointer to parent item */
    ActionHierarchyModelItems   _children;      /** Pointers to child items */
};

}
