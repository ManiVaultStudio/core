// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QUuid>
#include <QString>

namespace mv::workflow
{

/**
 * @brief Lightweight identifier for workflow entities that can produce outputs.
 *
 * Workflow handles allow jobs and contexts to refer to producer outputs without
 * retaining references to the producer object itself.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowHandle
{
public:

    /**
     * @brief Constructs an invalid workflow handle.
     */
    WorkflowHandle();

    /**
     * @brief Constructs a workflow handle.
     * @param id Workflow entity identifier.
     * @param name Human-readable workflow entity name.
     */
    explicit WorkflowHandle(QUuid id, QString name = {});

    /**
     * @brief Returns whether the handle is valid.
     * @return True if the handle contains a non-null identifier.
     */
    [[nodiscard]] bool isValid() const;

    /**
     * @brief Returns the workflow entity identifier.
     * @return Workflow entity identifier.
     */
    [[nodiscard]] QUuid getId() const;

    /**
     * @brief Returns the workflow entity name.
     * @return Human-readable workflow entity name.
     */
    [[nodiscard]] QString getName() const;

private:

    QUuid   _id;    /**< Workflow entity identifier. */
    QString _name;  /**< Human-readable workflow entity name. */
};

}
