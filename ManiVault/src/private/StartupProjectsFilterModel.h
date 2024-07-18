// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <models/SortFilterProxyModel.h>

/**
 * Plugin filter model class
 *
 * Sorting and filtering model for plugins models
 *
 * @author Thomas Kroes
 */
class StartupProjectsFilterModel : public mv::SortFilterProxyModel
{
public:

    /** No need for custom constructor (yet) */
    using mv::SortFilterProxyModel::SortFilterProxyModel;
};
