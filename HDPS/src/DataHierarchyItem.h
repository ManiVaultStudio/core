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

/** Vector of data hierarchy item pointers */
using DataHierarchyItems = QVector<DataHierarchyItem*>;

/**
 * Data hierarchy item class
 *
 * Represents a dataset as an item in the data hierarchy, see the AbstractDataHierarchyManager
 *
 * @author Thomas Kroes
 */
class DataHierarchyItem final : public mv::gui::WidgetAction
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

    /**
     * Establish whether the data hierarchy item has a parent
     * @return Boolean determining whether the data hierarchy item has a parent or not
     */
    bool hasParent() const;

    /**
     * Get pointer to parent hierarchy item
     * @return Pointer to parent hierarchy item if there is a parent otherwise nullptr
     */
    DataHierarchyItem* getParent() const;

protected:

    /**
     * Set pointer to parent hierarchy item to \p parent
     * @param parent Pointer to parent hierarchy item (root item if nullptr)
     */
    void setParent(DataHierarchyItem* parent);

public:

    /**
     * Walks up the hierarchy onwards from the data hierarchy item and returns all parents
     * @return Vector of pointers to parent items
     */
    DataHierarchyItems getParents() const;

    /**
     * Determine whether the data hierarchy item is a child of \p dataHierarchyItem
     * @param dataHierarchyItem Reference to possible parent data hierarchy item
     * @return Boolean determining if \p dataHierarchyItem is a parent
     */
    bool isChildOf(DataHierarchyItem& dataHierarchyItem) const;

    /**
     * Determine whether the data hierarchy item is a child of one or more \p dataHierarchyItems
     * @param dataHierarchyItems Vector of possible parent data hierarchy items
     * @return Boolean determining if it is a child of one or more \p dataHierarchyItems
     */
    bool isChildOf(DataHierarchyItems dataHierarchyItems) const;

    /**
     * Get children, possibly \p recursively
     * @param recursively Get all descendants
     * @return Vector of pointers to children
     */
    DataHierarchyItems getChildren(bool recursively = false) const;

    /**
     * Get number of children, possibly \p recursively
     * @param recursively Count recursively
     * @returm Number of children
     */
    std::uint32_t getNumberOfChildren(bool recursively = false) const;

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

    /** Get the dataset type */
    //DataType getDataType() const;

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

public: // Locked

    /**
     * Get locked status
     * @return Boolean determining if the dataset is locked or not
     */
    bool getLocked() const;

    /**
     * Set locked status to \p locked
     * @param locked Boolean determining whether the dataset is locked or not
     */
    void setLocked(bool locked);

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