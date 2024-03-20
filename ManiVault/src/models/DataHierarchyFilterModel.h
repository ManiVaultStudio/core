// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

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
class CORE_EXPORT DataHierarchyFilterModel : public QSortFilterProxyModel {
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
    gui::OptionsAction& getGroupFilterAction() { return _groupFilterAction; }
    gui::OptionsAction& getLockedFilterAction() { return _lockedFilterAction; }
    gui::OptionsAction& getDerivedFilterAction() { return _derivedFilterAction; }

private:
    gui::OptionsAction    _visibilityFilterAction;      /** For selecting whether visible and/or hidden datasets are shown or not */
    gui::OptionsAction    _groupFilterAction;           /** For selecting whether group and/or non-group datasets are shown or not */
    gui::OptionsAction    _lockedFilterAction;          /** For selecting whether locked and/or un-locked datasets are shown or not */
    gui::OptionsAction    _derivedFilterAction;         /** For selecting whether derived and/or non-derived datasets are shown or not */
};

}
