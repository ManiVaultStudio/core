// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractDataHierarchyManager.h"

#include <actions/WidgetAction.h>

#include <QObject>
#include <QMap>
#include <QString>

using namespace mv::gui;

namespace mv
{

class DataHierarchyManager final : public AbstractDataHierarchyManager
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    DataHierarchyManager(QObject* parent = nullptr);

    ~DataHierarchyManager() override;

    /** Perform manager startup initialization */
    void initialize() override;

    /** Resets the contents of the data hierarchy manager */
    void reset() override;

    /**
     * Get hierarchy item by \p datasetId
     * @param datasetId Dataset GUID
     * @return Reference to data hierarchy item
     */
    const DataHierarchyItem& getItem(const QString& datasetId) const override;

    /**
     * Get hierarchy item by \p datasetId
     * @param datasetId Dataset GUID
     * @return Reference to data hierarchy item
     */
    DataHierarchyItem& getItem(const QString& datasetId) override;

    /**
     * Get dataset children
     * @param dataHierarchyItem Reference to data hierarchy item
     * @param recursive Whether to get all children in a recursive manner
     * @return Children
     */
    DataHierarchyItems getChildren(DataHierarchyItem& dataHierarchyItem, const bool& recursive = true) override;

    /**
     * Get top-level items
     * @return Top-level items
     */
    DataHierarchyItems getTopLevelItems() override;

    /**
     * Set selected data hierarchy items
     * @param selectedItems Pointers to selected data hierarchy items
     */
    void selectItems(DataHierarchyItems& selectedItems) override;

protected:

    /**
     * Add a dataset to the hierarchy
     * @param dataset Smart pointer to dataset
     * @param parentDataset Smart pointer to parent dataset (if any)
     * @param visible Whether the dataset is visible in the gui
     */
    void addItem(Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, const bool& visible = true) override;

    /**
     * Removes data hierarchy for \p dataset from the data hierarchy
     * @param dataset Dataset to remove the data hierarchy item for
     */
    void removeItem(Dataset<DatasetImpl> dataset) override;

    /** Removes all items from the data hierarchy manager in a top-down manner */
    void removeAllItems() override;

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

private:
    std::vector<std::unique_ptr<DataHierarchyItem>>     _items;      /** Unique pointers to data hierarchy items */
};

}
