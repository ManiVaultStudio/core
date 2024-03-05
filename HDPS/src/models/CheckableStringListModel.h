// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QStringListModel>

namespace mv {

/**
 * Checkable string list model class
 *
 * Extends the standard string list model to include ability to check items
 *
 * @author Thomas Kroes
 */
class CheckableStringListModel : public QStringListModel {
public:

    /** No need for a custom constructor */
    using QStringListModel::QStringListModel;

    /**
     * Get flags for model \p index
     * @param index Model index to get the flags for
     * @return Item flags
     */
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    /**
     * Get data for model \p index and \p role
     * @param index Model index to get the data for
     * @param role The data role
     * @return Data in variant form
     */
    QVariant data(const QModelIndex& index, int role) const override;

    /**
     * Set data for model \p index and \p role to \p value
     * @param index Model index to set the data for
     * @param value The data value
     * @param role The data role
     * @return Whether setting the data was successful or not
     */
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

private:
    QSet<int>   _checkedItems;  /** Keeps track of the selected items */
};

}