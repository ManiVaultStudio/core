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
        Name,           /** Name of the action */
        Visible,        /** Whether the action is visible or not */
        Enabled,        /** Whether the action is enabled or not */
        Linkable,       /** Whether the action is linkable or not */

        _start = Name,
        _end = Linkable
    };

public:

    /**
     * Constructor
     * @param parent Parent action hierarchy item (if any)
     * @param action Pointer to non-owning action
     */
    explicit ActionHierarchyModelItem(WidgetAction* action, ActionHierarchyModelItem* parent = nullptr);

    /** Destructor */
    virtual ~ActionHierarchyModelItem();

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

    /**
     * Get data at column
     * @param column Column index
     * @param role Data role
     * @return Data in string format
     */
    QVariant getDataAtColumn(const std::uint32_t& column, int role = Qt::DisplayRole) const;

    /**
     * Get the action
     * @return Pointer to action
     */
    WidgetAction* getAction();

private:

    /**
     * Removes child
     * @param actionHierarchyModelItem Pointer to action hierarchy model item to remove
     */
    void removeChild(ActionHierarchyModelItem* actionHierarchyModelItem);

protected:
    WidgetAction*               _action;        /** Pointer to non-owning action */
    ActionHierarchyModelItem*   _parent;        /** Pointer to non-owning parent item */
    ActionHierarchyModelItems   _children;      /** Pointers to owning child items */
};

}
