// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractActionsModel.h"

namespace hdps
{

/**
 * Actions hierarchy model class
 *
 * Hierarchical actions model class
 *
 * @author Thomas Kroes
 */
class ActionsHierarchyModel final : public AbstractActionsModel
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object and \p rootAction
     * @param parent Pointer to parent object
     * @param rootAction When set, only list the root action and its descendants
     */
    ActionsHierarchyModel(QObject* parent, gui::WidgetAction* rootAction = nullptr);

    /** Initializes the model from the current state of the actions manager */
    void initialize() override;

protected:

    /**
     * Invoked when \p action is added to the actions manager
     * @param action Pointer to action that was added
     */
    void actionAddedToManager(gui::WidgetAction* action) override;

    /**
     * Invoked when \p action is about to be removed from the actions manager
     * @param action Pointer to action that was removed
     */
    void actionAboutToBeRemovedFromManager(gui::WidgetAction* action) override;

    /**
     * Invoked when \p publicAction is added to the actions manager
     * @param publicAction Pointer to public action that was added
     */
    void publicActionAddedToManager(gui::WidgetAction* publicAction) override;

    /**
     * Invoked when \p publicAction is about to be removed from the actions manager
     * @param publicAction Pointer to public action to remove
     */
    void publicActionAboutToBeRemovedFromManager(gui::WidgetAction* publicAction) override;

private:

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

private:
    gui::WidgetAction*  _rootAction;     /** Root of the action hierarchy */
};

}
