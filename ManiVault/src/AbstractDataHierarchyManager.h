// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractManager.h"

#include "DataHierarchyItem.h"

#include <QObject>
#include <QString>

#ifdef _DEBUG
    #define ABSTRACT_DATA_HIERARCHY_MANAGER_VERBOSE
#endif

namespace mv
{

/**
 * Abstract data hierarchy manager
 *
 * Base abstract data hierarchy manager class for organizing the dataset hierarchy.
 *
 * @ingroup mv_managers
 * @author Thomas Kroes
 */
class CORE_EXPORT AbstractDataHierarchyManager : public AbstractManager
{
    Q_OBJECT

public:

    /**
     * Construct manager with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    AbstractDataHierarchyManager(QObject* parent) :
        AbstractManager(parent, "DataHierarchy")
    {
    }

    /**
     * Get hierarchy item by \p datasetId
     * @param datasetId Dataset GUID
     * @return Pointer to data hierarchy item, nullptr if not found
     */
    virtual const DataHierarchyItem* getItem(const QString& datasetId) const = 0;

    /**
     * Get hierarchy item by \p datasetId
     * @param datasetId Dataset GUID
     * @return Pointer to data hierarchy item, nullptr if not found
     */
    virtual DataHierarchyItem* getItem(const QString& datasetId) = 0;

    /**
     * Get all items
     * @return List of items
     */
    virtual DataHierarchyItems getItems() const = 0;

    /**
     * Get dataset children
     * @param dataHierarchyItem Reference to data hierarchy item
     * @param recursive Whether to get all children in a recursive manner
     * @return Children
     */
    virtual DataHierarchyItems getChildren(DataHierarchyItem& dataHierarchyItem, const bool& recursive = true) = 0;

    /**
     * Get top-level items
     * @return Top-level items
     */
    virtual DataHierarchyItems getTopLevelItems() = 0;

public: // Item selection

    /**
     * Select data hierarchy \p items
     * @param items List of pointers to data hierarchy items to select
     * @param clear Clear the current selections before selection
     */
    virtual void select(DataHierarchyItems items, bool clear = true) = 0;

    /** Select all items */
    virtual void selectAll() = 0;

    /** Clear the item selection */
    virtual void clearSelection() = 0;

    /**
     * Get selected items
     * @return List of selected items
     */
    virtual DataHierarchyItems getSelectedItems() const = 0;

protected:

    /**
     * Add a dataset to the hierarchy
     * @param dataset Smart pointer to dataset
     * @param parentDataset Smart pointer to parent dataset (if any)
     * @param visible Whether the dataset is visible in the gui
     */
    virtual void addItem(Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, const bool& visible = true) = 0;

    /**
     * Removes data hierarchy for \p dataset from the data hierarchy
     * @param dataset Dataset to remove the data hierarchy item for
     */
    virtual void removeItem(Dataset<DatasetImpl> dataset) = 0;

    /** Removes all items from the data hierarchy manager in a top-down manner */
    virtual void removeAllItems() = 0;

public: // Serialization

    /**
     * Load from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override = 0;

    /**
     * Save to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override = 0;

signals:

    /**
     * Signals that \p dataHierarchyItem is added to the hierarchy manager
     * @param dataHierarchyItem Pointer to added data hierarchy item
     */
    void itemAdded(DataHierarchyItem* dataHierarchyItem);

    /**
     * Signals that \p dataHierarchyItem is about to be removed from the hierarchy manager
     * @param dataHierarchyItem Pointer to data hierarchy item which is about to be removed
     */
    void itemAboutToBeRemoved(DataHierarchyItem* dataHierarchyItem);

    /**
     * Signals that hierarchy item \p datasetId is removed from the hierarchy manager
     * @param datasetId GUID of the removed dataset
     */
    void itemRemoved(const QString& datasetId);

    /**
     * Signals that the parent of \p dataHierarchyItem changed
     * @param dataHierarchyItem Pointer to data hierarchy item of which the parent changed
     */
    void itemParentChanged(DataHierarchyItem* dataHierarchyItem);

    /**
     * Signals that the selected items changed
     * @param selectedItems Pointers to selected data hierarchy items
     */
    void selectedItemsChanged(DataHierarchyItems selectedItems);

    friend class DataManager;
};

}
