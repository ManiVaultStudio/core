// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QSortFilterProxyModel>

namespace mv {

/**
 * Raw data filter model class
 *
 * A class for filtering and sorting the raw data model
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT RawDataFilterModel : public QSortFilterProxyModel {
public:

    /** Constructor
     * @param parent Pointer to parent object
    */
    RawDataFilterModel(QObject* parent = nullptr);
};

}