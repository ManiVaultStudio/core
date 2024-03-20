// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <actions/OptionsAction.h>

#include <QSortFilterProxyModel>

namespace mv {

/**
 * Logging filter model
 *
 * Provides a sorting and filtering model for the logging model
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT LoggingFilterModel : public QSortFilterProxyModel
{
public:

    /** Constructor
     * @param parent Pointer to parent object
    */
    LoggingFilterModel(QObject* parent = nullptr);

    /**
     * Returns whether a give row with give parent is filtered out (false) or in (true)
     * @param row Row index
     * @param parent Parent index
     * @return Boolean indicating whether the item is filtered in or out
     */
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override;

    /**
     * Compares two cluster items
     * @param lhs Left cluster
     * @param rhs Right cluster
     */
    bool lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const override;

public:
    mv::gui::OptionsAction& getFilterTypeAction() { return _filterTypeAction; }

private:
    mv::gui::OptionsAction    _filterTypeAction;          /** Options action for filtering log record item type */
};

}
