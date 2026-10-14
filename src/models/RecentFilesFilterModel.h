// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "models/SortFilterProxyModel.h"

namespace mv {

/**
 * Recent files filter model class
 *
 * For filtering recent files model class.
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT RecentFilesFilterModel : public SortFilterProxyModel
{
public:

    /**
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
    */
    RecentFilesFilterModel(QObject* parent = nullptr);

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
     * Set source model to \p sourceModel
     * @param sourceModel Pointer to source model
     */
    void setSourceModel(QAbstractItemModel* sourceModel) override;
};

}
