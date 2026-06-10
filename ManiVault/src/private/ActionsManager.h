// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractActionsManager.h"

#include <models/ActionsListModel.h>

namespace mv
{

/**
 * Actions manager
 *
 * Extends the abstract actions manager class and adds functionality for serialization and action publishing
 *
 * @author Thomas Kroes
 */
class ActionsManager final : public AbstractActionsManager
{
    Q_OBJECT

public:

    /**
     * Construct manager with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    ActionsManager(QObject* parent);

    /** Reset when destructed */
    ~ActionsManager() override;

    /** Perform manager startup initialization */
    void initialize() override;

    /** Resets the contents of the actions manager */
    void reset() override;

    /**
     * Publish \p privateAction so that other private actions can connect to it
     * @param privateAction Pointer to private action to publish
     * @param name Name of the published widget action (if empty, a name choosing dialog will popup)
     * @param recursive Whether to also publish the child actions recursively
     * @param allowDuplicateName Boolean determining whether publishing will take place when a public with the same name already exists in the public actions database
     * @return Boolean determining whether the action is successfully published or not
     */
    bool publishPrivateAction(gui::WidgetAction* privateAction, const QString& name = "", bool recursive = true, bool allowDuplicateName = false) override;

public: // Models

    ActionsListModel& getActionsListModel() override;

public: // Serialization

    /**
     * Create a workflow that restores this object's state from a variant map.
     *
     * See Serializable::fromVariantMapWorkflow() for the full contract,
     * execution semantics, and implementation requirements.
     *
     * @param variantMap Serialized object state.
     * @return Workflow plan that restores the object state when executed.
     */
    UniqueWorkflowPlan fromVariantMapWorkflow(const QVariantMap& variantMap) override;

    /**
     * Create a workflow that serializes this object's state to a variant map.
     *
     * See Serializable::toVariantMapWorkflow() for the full contract,
     * execution semantics, and implementation requirements.
     *
     * @return Workflow plan that serializes the object state when executed.
     */
    UniqueWorkflowPlan toVariantMapWorkflow() const override;

private:
    ActionsListModel*    _actionsListModel;
};

}
