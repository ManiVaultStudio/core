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
 * @brief Identifies the output of a workflow node.
 *
 * A WorkflowHandle provides a lightweight, immutable reference to the output
 * produced by a workflow, stage, or job. It consists of a unique identifier
 * used to locate the associated execution context and an optional human-readable
 * name for diagnostics and debugging.
 *
 * Workflow handles are returned when constructing workflow plans and can later
 * be used to retrieve the corresponding output from a
 * WorkflowExecutionContext via takeOutput().
 *
 * Handles do not own or store the output value itself; they merely identify
 * where that output can be obtained.
 *
 * @authors BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowHandle
{
public:

    /**
     * @brief Constructs an invalid workflow handle.
     */
    WorkflowHandle();

    /**
     * @brief Constructs a workflow handle with the specified identifier and name.
     *
     * @param id Unique identifier of the referenced workflow output.
     * @param name Optional human-readable name of the referenced workflow node.
     */
    explicit WorkflowHandle(QUuid id, QString name = {});

    /**
     * @brief Returns whether this workflow handle references a valid workflow output.
     *
     * @return True if the handle contains a valid identifier; false otherwise.
     */
    [[nodiscard]] bool isValid() const;

    /**
     * @brief Returns the unique identifier of the referenced workflow output.
     *
     * @return Workflow output identifier.
     */
    [[nodiscard]] QUuid getId() const;

    /**
     * @brief Returns the human-readable name associated with this workflow handle.
     *
     * @return Workflow node name.
     */
    [[nodiscard]] QString getName() const;

private:
    QUuid   _id;    /** Unique identifier of the referenced workflow output */
    QString _name;  /** Human-readable name of the referenced workflow node */
};

}