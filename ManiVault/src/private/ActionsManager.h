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
     * Load manager from variant map
     * @param Variant map representation of the manager
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save manager to variant map
     * @return Variant map representation of the manager
     */
    QVariantMap toVariantMap() const override;

private:
    ActionsListModel*    _actionsListModel;
};

}
