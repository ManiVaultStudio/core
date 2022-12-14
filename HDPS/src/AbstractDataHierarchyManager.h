#pragma once

#include "AbstractManager.h"

#include "DataHierarchyItem.h"

#include <QObject>
#include <QString>

namespace hdps
{

/**
 * Abstract data hierarchy manager
 *
 * Base abstract data hierarchy manager class for organizing the dataset hierarchy.
 *
 * @author Thomas Kroes
 */
class AbstractDataHierarchyManager : public AbstractManager
{
    Q_OBJECT

public:

    /**
     * Add a dataset to the hierarchy
     * @param dataset Smart pointer to dataset
     * @param parentDataset Smart pointer to parent dataset (if any)
     * @param visible Whether the dataset is visible in the gui
     */
    virtual void addItem(Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, const bool& visible = true) = 0;

    /**
     * Removes a data hierarchy item (and its children recursively) from the data hierarchy
     * @param dataHierarchyItem Reference to data hierarchy item
     */
    virtual void removeItem(DataHierarchyItem& dataHierarchyItem) = 0;

    /** Removes all items from the data hierarchy manager in a top-down manner */
    virtual void removeAllItems() = 0;

    /**
     * Get hierarchy item by dataset globally unique identifier
     * @param datasetGuid Dataset GUID
     * @return Reference to data hierarchy item
     */
    virtual const DataHierarchyItem& getItem(const QString& datasetGuid) const = 0;

    /**
     * Get hierarchy item by dataset globally unique identifier
     * @param datasetGuid Dataset GUID
     * @return Reference to data hierarchy item
     */
    virtual DataHierarchyItem& getItem(const QString& datasetGuid) = 0;

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

    /**
     * Set selected data hierarchy items
     * @param selectedItems Pointers to selected data hierarchy items
     */
    virtual void selectItems(DataHierarchyItems& selectedItems) = 0;

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    virtual void fromVariantMap(const QVariantMap& variantMap) override = 0;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    virtual QVariantMap toVariantMap() const override = 0;

signals:

    /**
     * Signals that a hierarchy item is added to the hierarchy manager
     * @param dataHierarchyItem Reference to added data hierarchy item
     */
    void itemAdded(DataHierarchyItem& dataHierarchyItem);

    /**
     * Signals that a hierarchy item is about to be removed from the hierarchy manager
     * @param dataset Smart pointer to the about to be removed dataset
     */
    void itemAboutToBeRemoved(const Dataset<DatasetImpl>& dataset);

    /**
     * Signals that a hierarchy item is removed from the hierarchy manager
     * @param datasetGui GUID of the removed dataset
     */
    void itemRemoved(const QString& datasetGui);

    /**
     * Signals that a data hierarchy item is being loaded
     * @param loadingItem Reference to the data hierarchy item that is being loaded
     */
    void itemLoading(DataHierarchyItem& loadingItem);

    /**
     * Signals that a data hierarchy item has been loaded
     * @param loadedItem Reference to the data hierarchy item that has been loaded
     */
    void itemLoaded(DataHierarchyItem& loadedItem);

    /**
     * Signals that a data hierarchy item is being saved
     * @param savingItem Reference to the data hierarchy item that is being saved
     */
    void itemSaving(DataHierarchyItem& savingItem);

    /**
     * Signals that a data hierarchy item has been saved
     * @param loadedItem Reference to the data hierarchy item that has been saved
     */
    void itemSaved(DataHierarchyItem& savedItem);

    /**
     * Signals that the selected items changed
     * @param selectedItems Pointers to selected data hierarchy items
     */
    void selectedItemsChanged(DataHierarchyItems selectedItems);
};

}
