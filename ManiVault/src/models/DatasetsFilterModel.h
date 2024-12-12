// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "SortFilterProxyModel.h"

#include "Dataset.h"

#include "actions/ToggleAction.h"

namespace mv
{

class DatasetImpl;

/**
 * Datasets filter class
 *
 * Proxy model for filtering datasets
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT DatasetsFilterModel final : public SortFilterProxyModel
{
public:

    /** Filter function signature, the input are all datasets in the core and it returns the filtered datasets */
    using FilterFunction = std::function<bool(Dataset<DatasetImpl>)>;

public:

    /** Construct with parent \p parent object
     * @param parent Pointer to parent object
    */
    DatasetsFilterModel(QObject* parent = nullptr);

    /**
     * Returns whether \p row with \p parent is filtered out (false) or in (true)
     * @param row Row index
     * @param parent Parent index
     * @return Boolean indicating whether the item is filtered in or out
     */
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override;

    /**
     * Set datasets filter function (mode is set to StorageMode::Automatic)
     * @param datasetsFilterFunction Filter lambda (triggered when datasets are added and/or removed from the datasets model)
     */
    void setFilterFunction(const FilterFunction& filterFunction);

public: // Action getters

    gui::ToggleAction& getUseFilterFunctionAction() { return _useFilterFunctionAction; }

private:
    FilterFunction      _filterFunction;            /** Filter lambda */
    gui::ToggleAction   _useFilterFunctionAction;   /** Toggle the use of a filter function */
};

}
