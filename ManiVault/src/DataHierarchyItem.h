// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/WidgetAction.h"

#include "Dataset.h"

#include <QMap>
#include <QString>
#include <QDebug>
#include <QIcon>
#include <QVector>

namespace mv
{

class DatasetImpl;

/** List of data hierarchy item pointers */
class DataHierarchyItems : public QList<DataHierarchyItem*> {
public:

    using QList<DataHierarchyItem*>::QList;

    bool operator==(const DataHierarchyItems& rhs) const
    {
        for (auto dataHierarchyItem : *this)
            if (!rhs.contains(dataHierarchyItem))
                return false;

        return true;
    }
};

/**
 * Data hierarchy item class
 *
 * Represents a dataset as an item in the data hierarchy, see the AbstractDataHierarchyManager
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT DataHierarchyItem final : public gui::WidgetAction
{
    Q_OBJECT

public:

    /**
     * Construct with smart pointer to \p dataset, \p parentDataset and possible configure whether the item is \p visible and/or \p selected
     * @param dataset Smart pointer to dataset
     * @param parentDataset Smart pointer to parent dataset (if any)
     * @param visible Whether the dataset is visible
     * @param selected Whether the dataset is selected
     */
    DataHierarchyItem(Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, bool visible = true, bool selected = false);

public:

    /**
     * Get parent item
     * @return Pointer to parent item (nullptr if the item has not parent item)
     */
    DataHierarchyItem* getParent() const {
        return gui::WidgetAction::getParent<DataHierarchyItem>();
    }

    /**
     * Set pointer to parent hierarchy item to \p parent
     * @param parent Pointer to parent hierarchy item (root item if nullptr)
     */
    void setParent(DataHierarchyItem* parent) {
        gui::WidgetAction::setParent(parent);

        emit parentChanged(getParent());
    }

    /**
     * Get ancestors
     * @return Pointers to data hierarchy item ancestors (bottom-up)
     */
    DataHierarchyItems getAncestors() const {
        DataHierarchyItems ancestors;

        for (auto ancestor : WidgetAction::getAncestors<DataHierarchyItem>())
            ancestors << static_cast<DataHierarchyItem*>(ancestor);

        return ancestors;
    }

    /**
     * Get children, possibly \p recursively
     * @param recursively Get children recursively
     * @return Vector of pointers to child items
     */
    gui::WidgetActionsOfType<DataHierarchyItem> getChildren(bool recursively = false) const {
        return gui::WidgetAction::getChildren<DataHierarchyItem>(recursively);
    }

    /**
     * Get number of children
     * @param recursively Count recursively
     * @returm Number of children
     */
    std::uint32_t getNumberOfChildren(bool recursively = false) const {
        return gui::WidgetAction::getNumberOfChildren<DataHierarchyItem>(recursively);
    }

    /**
     * Establishes whether the item has any children
     * @return Boolean determining whether the item has any children
     */
    template<typename WidgetActionType = WidgetAction>
    bool hasChildren() const {
        return gui::WidgetAction::hasChildren<DataHierarchyItem>();
    }

    /**
     * Get the depth of the item w.r.t. its furthest ancestor
     * @return Item depth (root starts at zero)
     */
    std::int32_t getDepth() const {
        return gui::WidgetAction::getDepth<DataHierarchyItem>();
    }

    /**
     * Determine whether the item is a child of \p item
     * @param item Item to check for
     * @return Boolean determining whether \p item is an ancestor or not
     */
    bool isChildOf(DataHierarchyItem* item) const {
        return getAncestors().contains(item);
    }

    /**
     * Determine whether the item is a child of one of \p items
     * @param items Items to check for
     * @return Boolean determining whether the action is child of one of \p actions or not
     */
    bool isChildOf(gui::WidgetActionsOfType<DataHierarchyItem> items) const {
        for (const auto item : items)
            if (item != this && gui::WidgetAction::isChildOf<DataHierarchyItem>(item))
                return true;

        return false;
    }

public:

    /**
     * Set visibility to \p visible possibly \p recursively
     * @param visible Whether the data hierarchy item is visible or not
     * @param recursively Whether to set child data hierarchy items recursively or not
     */
    void setVisible(bool visible, bool recursively = true);

public: // Selection

    /**
     * Gets whether the hierarchy item is selected
     * @return Boolean determining whether the item is selected
     */
    bool isSelected() const;

    /**
     * Sets the selection status to \p selected and possibly \p clear the global selection
     * @param selected Whether the hierarchy item is selected
     * @param clear Whether to clear the global data hierarchy selection
     */
    void setSelected(bool selected, bool clear = true);

    /**
     * Selects the data hierarchy item
     * @param clear Whether to clear the global data hierarchy selection
     */
    void select(bool clear = true);

    /** De-selects the hierarchy item */
    void deselect();

public: // Miscellaneous

    /** Get the dataset */
    Dataset<DatasetImpl> getDataset();

    /** Get the dataset as \p DatasetType */
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

    /**
     * Get the dataset type
     * @return The dataset type
     */
    DataType getDataType() const;

public: // Actions

    /**
     * Attach \p widgetAction
     * @param widgetAction Reference to widget action to attach
     */
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

public: // Locked (facade for dataset)

    /**
     * Lock the dataset and possibly \p cache the current lock state
     * @param cache Whether to cache the current lock state
     */
    void lock(bool cache = false);

    /**
     * Unlock the dataset and possibly \p cache the current lock state
     * @param cache Whether to cache the current lock state
     */
    void unlock(bool cache = false);

    /**
     * Get whether the dataset is locked
     * @return Boolean indicating whether the dataset is locked
*
     */
    bool isLocked() const;

    /**
     * Set whether the dataset is \p locked and possibly \p cache the current lock state
     * @param locked Boolean indicating whether the dataset is locked
     * @param cache Whether to cache the current lock state
     */
    void setLocked(bool locked, bool cache = false);

    /** Restore the lock status to the cached value */
    void restoreLockedFromCache();

public: // Expansion

    /**
     * Get expanded status
     * @return Boolean indicating whether the item is expanded or not
     */
    bool isExpanded() const;

    /**
     * Set expanded status to \p expanded
     * @param expanded Boolean determining whether the dataset is expanded or not
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
     * Signals that item selection changed to \p selected
     * @param selected Whether the item is selected or not
     */
    void selectedChanged(bool selected);

    /**
     * Signals that \p widgetAction was attached to the data hierarchy item
     * @param widgetAction Reference to the widget action that was added
     */
    void actionAdded(mv::gui::WidgetAction& widgetAction);

    /** Signals that the set icon has changed */
    void iconChanged();

    /**
     * Signals that the dataset name changed to \p datasetName
     * @param datasetName New name of the dataset
     */
    void datasetNameChanged(const QString& datasetName);

    /**
     * Signals that the locked status changed to \p locked
     * @param locked Locked
     */
    void lockedChanged(bool locked);

    /**
     * Signals that the visibility changed to \p visibility
     * @param visibility Visibility
     */
    void visibilityChanged(bool visibility);

    /**
     * Signals that the expansion changed \p expanded
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
    bool                        _selected;      /** Whether the hierarchy item is selected */
    bool                        _expanded;      /** Whether the item is expanded or not (when it has children) */
    mv::gui::WidgetActions      _actions;       /** Attached widget actions */

protected:
    friend class DataHierarchyManager;
    friend class DataManager;
};

}

Q_DECLARE_METATYPE(mv::DataHierarchyItem)

inline const auto dataHierarchyItemMetaTypeId = qRegisterMetaType<mv::DataHierarchyItem*>("mv::DataHierarchyItem");