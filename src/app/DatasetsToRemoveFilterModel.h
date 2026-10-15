// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QSortFilterProxyModel>

class DatasetsToRemoveModel;

/**
 * Datasets to remove filter model class
 *
 * A class for filtering and sorting the datasets to remove model
 *
 * @author Thomas Kroes
 */
class DatasetsToRemoveFilterModel : public QSortFilterProxyModel {
public:

    /** Constructor
     * @param parent Pointer to parent object
    */
    DatasetsToRemoveFilterModel(QObject* parent = nullptr);

    /**
     * Returns whether a given row with give parent is filtered out (false) or in (true)
     * @param row Row index
     * @param parent Parent index
     */
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override;

private:

    /**
     * Get datasets to remove model
     * @return Source model cast as datasets source model
     */
    DatasetsToRemoveModel* getDatasetsToRemoveModel() const;
};
