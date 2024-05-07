// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QSortFilterProxyModel>

/**
 * Learning page videos filter model class
 *
 * Sorting and filtering model for the learning page videos model
 *
 * @author Thomas Kroes
 */
class LearningPageVideosFilterModel : public QSortFilterProxyModel
{
public:

    /** 
     * Construct with pointer to \p parent object
     * @param parent Pointer to parent object
    */
    LearningPageVideosFilterModel(QObject* parent = nullptr);
};
