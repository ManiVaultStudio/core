// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/OptionsAction.h>

#include <QSortFilterProxyModel>

/**
 * Workspace inventory filter model class
 *
 * Sorting and filtering model for the workspace inventory model
 *
 * @author Thomas Kroes
 */
class WorkspaceInventoryFilterModel : public QSortFilterProxyModel
{
public:

    /** 
     * Construct the filter model with \p parent
     * @param parent Pointer to parent object
    */
    WorkspaceInventoryFilterModel(QObject* parent = nullptr);

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
    mv::gui::OptionsAction& getFilterTypesAction() { return _filterTypesAction; }

private:
    mv::gui::OptionsAction    _filterTypesAction;      /** Workspace filter types action */
};
