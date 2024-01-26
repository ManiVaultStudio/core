// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractDataHierarchyModel.h"

namespace mv {

class DataHierarchyItem;

/**
 * Data hierarchy list model class
 *
 * Standard item model for managing the data hierarchy in list form
 *
 * @author Thomas Kroes
 */
class DataHierarchyListModel final : public AbstractDataHierarchyModel
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    explicit DataHierarchyListModel(QObject* parent = nullptr);

protected: // Drag-and-drop behavior

    /** Get supported drag actions */
    Qt::DropActions supportedDragActions() const override;

    /** Get the supported drop actions */
    Qt::DropActions supportedDropActions() const override;

public:

    /** Populate the model with data hierarchy items from the data hierarchy manager */
    void populateFromDataHierarchyManager();

private:

    /**
     * Add \p dataHierarchyItem to the model
     * @param dataHierarchyItem Pointer to the data hierarchy item to add
     */
    void addDataHierarchyModelItem(DataHierarchyItem* dataHierarchyItem);

    /**
     * Remove \p dataHierarchyItem from the model
     * @param dataHierarchyItem Pointer to the data hierarchy item to remove
     */
     void removeDataHierarchyModelItem(DataHierarchyItem* dataHierarchyItem);
};

}