// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "SortFilterProxyModel.h"

#include "actions/OptionsAction.h"

namespace mv {

/**
 * ColorSchemes filter model class
 *
 * Sorting and filtering model for color schemes
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ColorSchemesFilterModel : public SortFilterProxyModel
{
public:

    /** 
     * Construct the filter model with \p parent
     * @param parent Pointer to parent object
    */
    ColorSchemesFilterModel(QObject* parent = nullptr);

    /**
     * Returns whether a given row with give parent is filtered out (false) or in (true)
     * @param row Row index
     * @param parent Parent index
     */
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override;

public: // Action getters

    gui::OptionsAction& getModeFilterAction() { return _modeFilterAction; }

private:
    gui::OptionsAction  _modeFilterAction;  /** Filter action for the color scheme mode */
};

}