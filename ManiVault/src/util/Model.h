// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

namespace mv::util
{

/**
 * Get all persistent model indexes at \p columnIndex from \p model
 * @param model Pointer to the model
 * @param columnIndex Index of the column to retrieve model indexes from
 * @return List of persistent model indexes at the specified column index
 */
CORE_EXPORT QList<QPersistentModelIndex> getAllPersistentModelIndexesAtColumn(const QAbstractItemModel* model, std::int32_t columnIndex);

/**
 * Get all items at \p columnIndex from \p model
 * @param model Pointer to the model
 * @param columnIndex Index of the column to retrieve items from
 * @return List of standard items at the specified column index
 */
CORE_EXPORT QList<QStandardItem*> getAllStandardItemsAtColumn(const QStandardItemModel* model, std::int32_t columnIndex);

}
