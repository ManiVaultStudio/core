// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "SortFilterProxyModel.h"

#include "actions/ToggleAction.h"

namespace mv {

/**
 * Plugin filter model class
 *
 * Sorting and filtering model for plugins models
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT PluginsFilterModel : public SortFilterProxyModel
{
public:

    /** Filter function signature, the input are all datasets in the core and it returns the filtered datasets */
    using FilterFunction = std::function<bool(plugin::Plugin*)>;

public:

    /** 
     * Construct the filter model with \p parent
     * @param parent Pointer to parent object
    */
    PluginsFilterModel(QObject* parent = nullptr);

    /**
     * Returns whether \p row with \p parent is filtered out (false) or in (true)
     * @param row Row index
     * @param parent Parent index
     * @return Boolean indicating whether the item is filtered in or out
     */
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override;

    /**
     * Compares two model indices plugin \p lhs with \p rhs
     * @param lhs Left-hand model index
     * @param rhs Right-hand model index
     */
    bool lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const override;

    /**
     * Set plugins filter function to \p filterFunction
     * @param filterFunction Filter lambda (triggered when plugins are added and/or removed)
     */
    void setFilterFunction(const FilterFunction& filterFunction);

    /**
     * Get plugin types to filter
     * @return Plugin types
     */
    plugin::Types getFilterPluginTypes() const;

    /**
     * Set plugin filter types to \p filterPluginTypes
     * @param filterPluginTypes Plugin filter types
     */
    void setFilterPluginTypes(const plugin::Types& filterPluginTypes);

public:

    gui::ToggleAction& getUseFilterFunctionAction() { return _useFilterFunctionAction; }
    gui::ToggleAction& getInstantiatedPluginsOnlyAction() { return _instantiatedPluginsOnlyAction; }

private:
    bool hasPluginInstances(const QModelIndex& index, int level = 0) const;

private:
    FilterFunction      _filterFunction;                    /** Filter lambda */
    plugin::Types       _filterPluginTypes;                 /** Filter on plugin types */
    gui::ToggleAction   _useFilterFunctionAction;           /** Toggle the use of a filter function */
    gui::ToggleAction   _instantiatedPluginsOnlyAction;     /** Show only instantiated plugins */
};

}