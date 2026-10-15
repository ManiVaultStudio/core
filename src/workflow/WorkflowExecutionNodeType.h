// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QString>

namespace mv::workflow
{

/**
 * @brief Defines the semantic role of a workflow node.
 *
 * Workflow execution nodes represent different levels and types of execution entities
 * within a workflow plan, such as entire workflows, nested workflows, stages, and jobs.
 * The node type determines how the node should be interpreted, visualized, reported,
 * and aggregated for progress reporting.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
enum class WorkflowExecutionNodeType
{
    Workflow,          /**< Top-level workflow execution context. */
    NestedWorkflow,    /**< Workflow executed as part of another workflow. */
    SequentialStage,   /**< Sequential execution stage. */
    ParallelStage,     /**< Parallel execution stage. */
    Job,               /**< Individual executable job. */
    Undefined          /**< Undefined or unknown context type. */
};

/**
 * @brief Converts a node type name to an enum value.
 *
 * @param typeName Case-sensitive workflow execution node type name.
 * @return Matching node type, or WorkflowExecutionNodeType::Undefined.
 */
[[nodiscard]] CORE_EXPORT WorkflowExecutionNodeType getWorkflowExecutionNodeType(const QString& typeName);

/**
 * @brief Converts a node type enum value to a name.
 *
 * @param type Workflow execution node type.
 * @return Name for the provided node type, or "Undefined" for unknown values.
 */
[[nodiscard]] CORE_EXPORT QString getWorkflowExecutionNodeTypeName(WorkflowExecutionNodeType type);

}
