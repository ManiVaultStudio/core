// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QStringListModel>

namespace mv {

/**
 * Checkable string list model class
 *
 * Extends the standard string list model to include ability to check items
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT CheckableStringListModel final : public QStringListModel{
public:

    using StringIndicesSet = QSet<std::int32_t>;
    using StringIndicesList = QList<std::int32_t>;

private:

    /**
     * We want to reset our internal CheckableStringListModel#_checkedItems every time the strings are changed.
     * We make the CheckableStringListModel::setStringList(...) private and replace it with
     * CheckableStringListModel::setStrings(...) so that we have control over the internals. This also prevents
     * accidental misuse of QStringListModel::setStringList(...), which would lead to problems with checked items.
     * Note: Underneath we still call QStringListModel::setStringList(...)
     */
    using QStringListModel::setStringList;

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

    /**
     * Sets the strings in the model to \p strings
     * @param strings New strings
     */
    void setStrings(const QStringList& strings);

    /**
     * Get checked strings
     * @return String list of checked items
     */
    QStringList getCheckedStrings() const;

    /**
     * Get checked indices set
     * @return Set of checked indices
     */
    StringIndicesSet getCheckedIndicesSet() const;

    /**
     * Get checked indices list
     * @return List of checked indices
     */
    StringIndicesList getCheckedIndicesList() const;

    /**
     * Set checked indices set to \p checkedStringIndicesSet
     * @param checkedStringIndicesSet Set of checked string indices
     */
    void setCheckedIndicesSet(const StringIndicesSet& checkedStringIndicesSet);

    /**
     * Set checked indices from \p checkedStrings
     * @param checkedStrings List of checked strings
     */
    void setCheckedIndicesFromStrings(const QStringList& checkedStrings);

private:
    StringIndicesSet   _checkedItems;  /** Keeps track of the selected items */
};

}