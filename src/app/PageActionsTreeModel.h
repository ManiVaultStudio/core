// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractPageActionsModel.h"

/**
 * Page actions tree model class
 *
 * For hierarchical page actions.
 *
 * @author Thomas Kroes
 */
class PageActionsTreeModel final : public AbstractPageActionsModel
{
public:

    /** No need for custom constructor */
    using AbstractPageActionsModel::AbstractPageActionsModel;

    /**
     * Add \p pageAction
     * @param pageAction Shared pointer to page action
     */
    void add(const PageActionSharedPtr& pageAction) override;

    /**
     * Add \p pageAction
     * @param pageAction Shared pointer to page action
     */
    void remove(const PageActionSharedPtr& pageAction) override;
};
