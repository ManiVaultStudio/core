#pragma once

#include "DataHierarchyItem.h"

#include <actions/WidgetAction.h>

#include <QObject>
#include <QMap>
#include <QString>
#include <QDebug>

using namespace hdps::gui;

namespace hdps
{

class DataManager;

class DataHierarchyManager : public WidgetAction
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    DataHierarchyManager(QObject* parent = nullptr);

    /**
     * Add a dataset to the hierarchy
     * @param dataset Smart pointer to dataset
     * @param parentDataset Smart pointer to parent dataset (if any)
     * @param visible Whether the dataset is visible in the gui
     */
    void addItem(Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, const bool& visible = true);

    /**
     * Removes a data hierarchy item (and its children recursively) from the data hierarchy
     * @param dataHierarchyItem Reference to data hierarchy item
     * @return Vector of datasets that were referenced by the data hierarchy items
     */
    Datasets removeItem(DataHierarchyItem& dataHierarchyItem);

    /**
     * Removes all items from the data hierarchy manager in a top-down manner
     * @return Vector of datasets that were referenced by the data hierarchy items
     */
    Datasets removeAllItems();

    /**
     * Get hierarchy item by dataset globally unique identifier
     * @param datasetGuid Dataset GUID
     * @return Reference to data hierarchy item
     */
    const DataHierarchyItem& getItem(const QString& datasetGuid) const;

    /**
     * Get hierarchy item by dataset globally unique identifier
     * @param datasetGuid Dataset GUID
     * @return Reference to data hierarchy item
     */
    DataHierarchyItem& getItem(const QString& datasetGuid);

    /**
     * Get dataset children
     * @param dataHierarchyItem Pointer to data hierarchy item
     * @param recursive Whether to get all children in a recursive manner
     * @return Children
     */
    DataHierarchyItems getChildren(DataHierarchyItem* dataHierarchyItem, const bool& recursive = true);

    /**
     * Set selected data hierarchy items
     * @param selectedItems Pointers to selected data hierarchy items
     */
    void selectItems(DataHierarchyItems& selectedItems);

protected:

    /**
     * Remove data hierarchy item from the selection
     * @param dataHierarchyItem Reference to data hierarchy item
     */
    void removeSelectedItem(DataHierarchyItem& dataHierarchyItem);

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

private:
    DataHierarchyItems    _items;           /** Shared pointers to data hierarchy items */
    DataHierarchyItems    _selectedItems;   /** Shared pointers to selected data hierarchy items */
};

}
