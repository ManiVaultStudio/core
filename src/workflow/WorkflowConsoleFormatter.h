// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowProgressNode.h"

#include "util/SeverityLevel.h"

namespace mv::workflow
{

/**
 * @brief Formats workflow-related log messages for console output.
 *
 * WorkflowConsoleFormatter converts workflow execution events, progress trees,
 * and structured log messages into human-readable console output. The formatter
 * is responsible for rendering workflow progress hierarchies, applying severity
 * formatting, and generating consistent textual representations of workflow
 * execution state.
 *
 * Progress trees are rendered from WorkflowProgressNode snapshots and may be
 * depth-limited to reduce verbosity. Log messages can include contextual
 * information such as severity, source location, and additional details.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowConsoleFormatter
{
public:

    /**
     * @brief Formats a workflow log message.
     *
     * Creates a formatted textual representation of a workflow log entry,
     * including its severity level, source location, message text, and
     * optional structured details.
     *
     * @param severity Severity level associated with the message.
     * @param text Message text.
     * @param location Location associated with the message.
     * @param details Additional structured message details.
     * @param maxDepth Maximum depth for rendering nested detail structures.
     * @return Formatted console output string.
     */
    [[nodiscard]] static QString format(util::SeverityLevel severity, const QString& text, const QString& location, const QVariantMap& details, std::uint32_t maxDepth = std::numeric_limits<std::uint32_t>::max());

    /**
     * @brief Formats a workflow progress tree.
     *
     * Produces a human-readable textual representation of a workflow progress
     * hierarchy from a progress tree snapshot.
     *
     * @param root Root progress node snapshot.
     * @return Formatted progress tree string.
     */
    [[nodiscard]] static QString formatProgressTree(const WorkflowProgressNode::Snapshot& root);

private:

    /**
     * @brief Appends a progress node and its descendants to a list of output lines.
     *
     * Traverses the supplied progress node recursively and appends formatted
     * lines representing the node hierarchy.
     *
     * @param lines Output lines to append to.
     * @param node Progress node snapshot to format.
     * @param depth Current hierarchy depth.
     */
    static void appendProgressNode(QStringList& lines, const WorkflowProgressNode::Snapshot& node, int depth);

    /**
     * @brief Returns a textual representation of a workflow status.
     *
     * Converts a workflow progress status value into a human-readable name.
     *
     * @param status Workflow status.
     * @return Human-readable status name.
     */
    [[nodiscard]] static QString statusName(WorkflowProgressNode::Status status);

    /**
     * @brief Returns a string-based icon associated with an event type.
     *
     * Maps an event identifier to a textual or Unicode icon used when
     * formatting workflow output.
     *
     * @param event Event identifier.
     * @return Event icon.
     */
    [[nodiscard]] static QString iconForEvent(const QString& event);

    /**
     * @brief Returns a human-readable label for an entity type.
     *
     * Converts an entity identifier into a display label suitable for
     * formatted console output.
     *
     * @param entity Entity identifier.
     * @return Human-readable entity label.
     */
    [[nodiscard]] static QString labelForEntity(const QString& entity);
};

}
