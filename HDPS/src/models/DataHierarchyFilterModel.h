// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QSortFilterProxyModel>

/**
 * Data hierarchy filter model class
 *
 * A class for filtering and sorting the data hierarchy model
 *
 * @author Thomas Kroes
 */
class DataHierarchyFilterModel : public QSortFilterProxyModel {
public:

    /** Constructor
     * @param parent Pointer to parent object
    */
    DataHierarchyFilterModel(QObject* parent = nullptr);

    /**
     * Returns whether a given row with give parent is filtered out (false) or in (true)
     * @param row Row index
     * @param parent Parent index
     */
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override;

    /** Get/set whether hidden datasets should be filtered or not */
    bool getFilterHidden() const;
    void setFilterHidden(bool filterHidden);

private:
    bool    _filterHidden;        /** Whether hidden datasets are show or not */
};
