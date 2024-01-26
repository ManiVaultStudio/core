// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/OptionsAction.h"

#include <QSortFilterProxyModel>

namespace mv {

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

public: // Action getters

    gui::OptionsAction& getVisibilityFilterAction() { return _visibilityFilterAction; }

private:
    gui::OptionsAction    _visibilityFilterAction;   /** For selecting  whether visible and/or hidden datasets are shown or not */
};

}