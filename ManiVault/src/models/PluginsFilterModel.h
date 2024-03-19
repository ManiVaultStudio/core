// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/ToggleAction.h"

#include <QSortFilterProxyModel>

namespace mv {

/**
 * Plugin filter model class
 *
 * Sorting and filtering model for plugins models
 *
 * @author Thomas Kroes
 */
class PluginsFilterModel : public QSortFilterProxyModel
{
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

public:
    mv::gui::ToggleAction& getInstantiatedPluginsOnlyAction() { return _instantiatedPluginsOnlyAction; }

private:
    bool hasPluginInstances(const QModelIndex& index, int level = 0) const;

private:
    mv::gui::ToggleAction     _instantiatedPluginsOnlyAction;      /** Show only instantiated plugins */
};

}
