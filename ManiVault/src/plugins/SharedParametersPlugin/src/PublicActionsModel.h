// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "models/AbstractActionsModel.h"

namespace mv
{

/**
 * Actions hierarchy model class
 *
 * Hierarchical actions model class for management of public actions (parameters)
 *
 * @author Thomas Kroes
 */
class PublicActionsModel final : public AbstractActionsModel
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    PublicActionsModel(QObject* parent);

    /** Initializes the model from the current state of the actions manager */
    void initialize() override;

protected:

    /**
     * Helper method which is invoked when \p action is added to the actions manager
     * @param action Pointer to action that was added
     */
    void actionAddedToManager(gui::WidgetAction* action) override;

    /**
     * Helper method which is invoked when \p action is about to be removed from the actions manager
     * @param action Pointer to action that was removed
     */
    void actionAboutToBeRemovedFromManager(gui::WidgetAction* action) override;

    /**
     * Helper method which is invoked when \p publicAction is added to the actions manager
     * @param publicAction Pointer to public action that was added
     */
    void publicActionAddedToManager(gui::WidgetAction* publicAction) override;

    /**
     * Helper method which is invoked when \p publicAction is about to be removed from the actions manager
     * @param publicAction Pointer to public action to remove
     */
    void publicActionAboutToBeRemovedFromManager(gui::WidgetAction* publicAction) override;

public:

    /**
     * Add \p action to the model
     * @param action Pointer to action to add
     */
    void addAction(gui::WidgetAction* action);

    /**
     * Remove \p action from the model
     * @param action Pointer to action to remove
     */
    void removeAction(gui::WidgetAction* action);

    /**
     * Add \p publicAction to the model
     * @param publicAction Pointer to public action to add
     */
    void addPublicAction(gui::WidgetAction* publicAction);

    /**
     * Remove \p publicAction from the model
     * @param publicAction Pointer to public action to remove
     */
    void removePublicAction(gui::WidgetAction* publicAction);
};

}
