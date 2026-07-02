// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QString>

namespace mv::workflow
{

/**
 * @brief Describes the semantic role of a workflow execution node within the workflow execution hierarchy.
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
    Workflow,          /**< Top-level workflow execution context */
    NestedWorkflow,    /**< Workflow executed as part of another workflow */
    SequentialStage,   /**< Sequential execution stage */
    ParallelStage,     /**< Parallel execution stage */
    Job,               /**< Individual executable job */
    Undefined          /**< Undefined or unknown context type */
};

/**
 * @brief Converts a node type name to a workflow execution node type.
 * @param typeName Case-sensitive node type name.
 * @return Matching node type, or WorkflowExecutionNodeType::Undefined for unknown names.
 */
[[nodiscard]] CORE_EXPORT WorkflowExecutionNodeType getWorkflowExecutionNodeType(const QString& typeName);

/**
 * @brief Converts a workflow execution node type to its name.
 * @param type Node type to convert.
 * @return Node type name, or "Undefined" for unknown values.
 */
[[nodiscard]] CORE_EXPORT QString getWorkflowExecutionNodeTypeName(WorkflowExecutionNodeType type);

}
