// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include "WorkflowResult.h"

#include <QObject>
#include <QHash>
#include <QUuid>

namespace mv::workflow
{

/**
 * @brief Thread-safe registry for workflow results.
 *
 * The WorkflowResultRegistry provides a central repository for storing and
 * retrieving workflow results by their unique identifier. It is primarily used
 * to temporarily share workflow results between components without requiring
 * direct ownership or lifetime management.
 *
 * Results are stored as shared pointers and remain available until explicitly
 * removed from the registry.
 *
 * The registry is implemented as a thread-safe singleton.
 */
class CORE_EXPORT WorkflowResultRegistry : public QObject
{
    Q_OBJECT

public:

    /** @return Thread-safe singleton workflow result registry. */
    static WorkflowResultRegistry& instance();

    /**
     * @brief Add a workflow result to the registry.
     * @param result Shared pointer to the workflow result.
     * @return Unique identifier assigned to the stored result.
     */
    QUuid add(SharedWorkflowResult result);

    /**
     * @brief Retrieve a workflow result from the registry.
     * @param id Identifier returned by add().
     * @return The associated workflow result, or nullptr if no result with the
     * specified identifier exists.
     */
    SharedWorkflowResult get(const QUuid& id) const;

    /**
     * @brief Remove a workflow result from the registry.
     * @param id Identifier of the workflow result to remove.
     */
    void remove(const QUuid& id);

private:

    mutable QMutex                      _mutex;     /**< Protects concurrent access to the registry */
    QHash<QUuid, SharedWorkflowResult>  _results;   /**< Registered workflow results indexed by identifier */
};

}
