// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QString>

namespace mv::workflow
{

/**
 * @brief Semantic workflow execution node role.
 *
 * Workflow execution nodes represent different levels and types of execution entities
 * within a workflow plan, such as entire workflows, nested workflows, stages, and jobs.
 * The node type determines how the node should be interpreted, visualized, reported,
 * and aggregated for progress reporting.
 */
enum class WorkflowExecutionNodeType
{
    Workflow,          /**< Top-level workflow execution context */
    NestedWorkflow,    /**< Workflow executed as part of another workflow */
    SequentialStage,   /**< Sequential execution stage */
    ParallelStage,     /**< Parallel execution stage */
    Job,               /**< Individual executable job */
    Undefined          /**< Undefined or unknown context type */
};

/**
 * @brief Converts a node type name to an enum value.
 *
 * @param typeName The string representation of the workflow execution node type to convert. This should be one of the defined type names, such as "Workflow", "NestedWorkflow", "SequentialStage", "ParallelStage", or "Job". The comparison is case-sensitive.
 * @return The corresponding WorkflowExecutionNodeType enum value for the provided string. For example, if the input is "Workflow", the output would be WorkflowExecutionNodeType::Workflow. If the input string does not match any known workflow execution context type, the function returns WorkflowExecutionNodeType::Undefined.
 */
CORE_EXPORT WorkflowExecutionNodeType getWorkflowExecutionNodeType(const QString& typeName);

/**
 * @brief Converts a node type enum value to a name.
 *
 * @param type The WorkflowExecutionNodeType enum value to convert.
 * @return The string representation of the provided node type. Returns "Undefined" for unknown values.
 */
CORE_EXPORT QString getWorkflowExecutionNodeTypeName(WorkflowExecutionNodeType type);

}
