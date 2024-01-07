// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "DataType.h"
#include "actions/WidgetAction.h"
#include "event/Event.h"

#include <QObject>
#include <QMap>
#include <QString>
#include <QDebug>
#include <QIcon>
#include <QTimer>
#include <QBitArray>
#include <QVector>

namespace mv
{

class DatasetImpl;

/** Vector of data hierarchy item pointers */
using DataHierarchyItems = QVector<DataHierarchyItem*>;

/**
 * Data hierarchy item class
 *
 * Represents a dataset as an item in a dataset hierarchy
 *
 * @author Thomas Kroes
 */
class DataHierarchyItem final : public mv::gui::WidgetAction
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param dataset Smart pointer to dataset
     * @param parentDataset Smart pointer to parent dataset (if any)
     * @param visible Whether the dataset is visible
     * @param selected Whether the dataset is selected
     */
    DataHierarchyItem(Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, const bool& visible = true, const bool& selected = false);

    /** Destructor */
    ~DataHierarchyItem();

    /** Returns whether the data hierarchy item has a parent */
    bool hasParent() const;

    /**
     * Get pointer to parent hierarchy item
     * @return Pointer to parent hierarchy item if there is a parent otherwise nullptr
     */
    DataHierarchyItem* getParent() const;

protected:

    /**
     * Set pointer to parent hierarchy item to \p parent
     * @return Pointer to parent hierarchy item (root item if nullptr)
     */
    void setParent(DataHierarchyItem* parent);

public:

    /**
     * Walks up the hierarchy of the data hierarchy item and returns all parents
     * @return Parent items
     */
    DataHierarchyItems getParents() const;

    /**
     * Determine whether the data hierarchy item is a child of \p dataHierarchyItem
     * @param dataHierarchyItem Possible parent data hierarchy item
     * @return Boolean determining if \p dataHierarchyItem is a parent
     */
    bool isChildOf(DataHierarchyItem& dataHierarchyItem) const;

    /**
     * Determine whether the data hierarchy item is a child of one or more \p dataHierarchyItems
     * @param dataHierarchyItems Possible parent data hierarchy items
     * @return Boolean determining if it is a child of one or more \p dataHierarchyItems
     */
    bool isChildOf(DataHierarchyItems dataHierarchyItems) const;

    /**
     * Get children, possibly \p recursively
     * @param recursively Get all descendants
     * @return Children
     */
    DataHierarchyItems getChildren(const bool& recursively = false) const;

    /**
     * Get number of children, possibly \p recursively
     * @param recursively Count recursively
     * @returm Number of children
     */
    std::uint32_t getNumberOfChildren(const bool& recursively = false) const;

    /**
     * Establishes whether the item has any children
     * @return Boolean determining whether the item has any children
     */
    bool hasChildren() const;

    /**
     * Get the depth of the data hierarchy item
     * @return Item depth (root starts at zero)
     */
    std::int32_t getDepth() const;

    /**
     * Set visibility
     * @param visible Whether the data hierarchy item is visible or not
     */
    void setVisible(bool visible);

public: // Selection

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

protected:

    /**
     * Remove \p dataHierarchyItem from the children
     * @param dataHierarchyItem Pointer to child data hierarchy item to remove
     */
    void removeChild(DataHierarchyItem* dataHierarchyItem);

public: // Hierarchy

    /**
     * Add \p dataHierarchyItem as a child item
     * @param dataHierarchyItem Pointer to child data hierarchy item to add
     */
    void addChild(DataHierarchyItem* dataHierarchyItem);

public: // Miscellaneous

    /** Get the dataset */
    Dataset<DatasetImpl> getDataset();

    /** Get the dataset */
    template<typename DatasetType>
    Dataset<DatasetType> getDataset() const {
        return Dataset<DatasetType>(const_cast<DataHierarchyItem*>(this)->getDataset().get<DatasetType>());
    };

    /**
     * Get reference to dataset smart pointer
     * @return Reference to dataset smart pointer
     */
    Dataset<DatasetImpl>& getDatasetReference();

    /**
     * Get reference to dataset smart pointer of a specific type
     * @return Reference to dataset smart pointer of a specific type
     */
    template<typename DatasetType>
    Dataset<DatasetType>& getDatasetReference() const {
        return Dataset<DatasetType>(const_cast<DataHierarchyItem*>(this)->getDataset().get<DatasetType>());
    };

    /** Get the dataset type */
    DataType getDataType() const;

public: // Actions

    /** Add action */
    void addAction(mv::gui::WidgetAction& widgetAction);

    /** Returns list of shared action widgets*/
    mv::gui::WidgetActions getActions() const;

    /**
     * Get the context menu
     * @param parent Parent widget
     * @return Context menu
     */
    QMenu* getContextMenu(QWidget* parent = nullptr) override;

    /**
     * Populates existing menu with actions menus
     * @param contextMenu Context menu to populate
     */
    void populateContextMenu(QMenu* contextMenu);;

public: // Locked

    /** Get locked status */
    bool getLocked() const;

    /**
     * Set locked status
     * @param locked Whether the dataset is locked
     */
    void setLocked(const bool& locked);

public: // Expanded

    /**
     * Get expanded status
     * @return Boolean indicating whether the item is expanded or not
     */
    bool isExpanded() const;

    /**
     * Set expanded status
     * @param expanded Whether the dataset is expanded
     */
    void setExpanded(bool expanded);

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

signals:

    /**
     * Signals that the item got selected
     * @param selected Whether the item is selected
     */
    void selectionChanged(const bool& selected);

    /**
     * Signals that a widget action was added
     * @param widgetAction Widget action that was added
     */
    void actionAdded(mv::gui::WidgetAction& widgetAction);

    /** Signals that the set icon has changed */
    void iconChanged();

    /**
     * Signals that the dataset name changed
     * @param datasetName New name of the dataset
     */
    void datasetNameChanged(const QString& datasetName);

    /**
     * Signals that the locked status changed
     * @param locked Locked
     */
    void lockedChanged(bool locked);

    /**
     * Signals that the visibility status changed
     * @param visibility Visibility
     */
    void visibilityChanged(bool visibility);

    /**
     * Signals that the expansion status changed
     * @param expanded Whether the item is expanded or not
     */
    void expandedChanged(bool expanded);

    /**
     * Signals that the parent changed to \p parent
     * @param parent Pointer to parent data hierarchy item (might be nullptr)
     */
    void parentChanged(DataHierarchyItem* parent);

protected:
    Dataset<DatasetImpl>        _dataset;       /** Smart pointer to dataset */
    DataHierarchyItem*          _parent;        /** Pointer to parent data hierarchy item */
    DataHierarchyItems          _children;      /** Pointers to child items (if any) */
    bool                        _selected;      /** Whether the hierarchy item is selected */
    bool                        _expanded;      /** Whether the item is expanded or not (when it has children) */
    mv::gui::WidgetActions      _actions;       /** Widget actions */

protected:
    friend class DataHierarchyManager;
    friend class DataManager;
};

}
