// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractActionsModel.h"

namespace mv
{

/**
 * Actions list model class
 *
 * List model for actions derived from abstract actions model class
 * 
 * When initialized with a root action, it lists the root action and its descendants,
 * otherwise it just lists all the actions from the actions manager.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ActionsListModel final : public AbstractActionsModel
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object and \p rootAction
     * @param parent Pointer to parent object
     * @param rootAction When set, only list the root action and its descendants
     */
    ActionsListModel(QObject* parent, gui::WidgetAction* rootAction = nullptr);

    /** Initializes the model from the current state of the actions manager */
    void initialize() override;

    /**
     * Get root action
     * @return Pointer to root action (if any)
     */
    gui::WidgetAction* getRootAction();

    /**
     * Set root action to \p rootAction 
     * @param rootAction When set, only list the root action and its descendants
     */
    void setRootAction(gui::WidgetAction* rootAction);

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

private:

    /**
     * Add \p action to the model
     * @param action Pointer to action to add
     */
    void addAction(gui::WidgetAction* action);

signals:
    
    /**
     * Signals that the root action changed to \p rootAction
     * @param rootAction Pointer to root action (if any)
     */
    void rootActionChanged(gui::WidgetAction* rootAction);

private:
    gui::WidgetAction*      _rootAction;    /** List this action and its descendants, otherwise list all actions from the actions manager */
};

}
