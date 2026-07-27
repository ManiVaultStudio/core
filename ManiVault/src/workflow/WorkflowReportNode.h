// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowMessage.h"

#include <QString>
#include <QDateTime>
#include <QMutex>

namespace mv::workflow
{

/**
 * @brief Node in a hierarchical workflow execution report.
 *
 * Each report node stores messages for one workflow entity and owns child nodes
 * for nested workflows, stages, or jobs. Access is synchronized because report
 * nodes may be updated from multiple workflow worker threads.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowReportNode
{
public:

    Q_DISABLE_COPY_MOVE(WorkflowReportNode)

    /** Shared pointer type for workflow report nodes. */
    using SharedWorkflowReportNode = std::shared_ptr<WorkflowReportNode>;

    /** Collection of child workflow report nodes. */
    using SharedWorkflowReportNodes = std::vector<SharedWorkflowReportNode>;

    /**
     * @brief Constructs a report node.
     * @param name Human-readable report node name.
     */
    explicit WorkflowReportNode(QString name);

    /**
     * @brief Creates a child report node.
     * @param name Human-readable child node name.
     * @return Shared child report node.
     */
    [[nodiscard]] SharedWorkflowReportNode createChild(const QString& name);

    /**
     * @brief Adds a message to this report node.
     * @param level Message severity level.
     * @param emitter Component that emitted the message.
     * @param text Message text.
     * @param location Source or workflow location associated with the message.
     * @param details Additional structured details.
     * @param contextId Optional workflow execution context identifier.
     * @param parentContextId Optional parent workflow execution context identifier.
     */
    void addMessage(util::SeverityLevel level, const QString& emitter, const QString& text, const QString& location, const QVariantMap& details = {}, const QString& contextId = "", const QString& parentContextId = "");

    /**
     * @brief Returns the report node name.
     * @return Human-readable report node name.
     */
    [[nodiscard]] QString getName() const;

    /**
     * @brief Returns messages emitted for this node.
     * @return Copy of the message list.
     */
    [[nodiscard]] WorkflowMessages getMessages() const;

    /**
     * @brief Returns direct child report nodes.
     * @return Copy of the child node list.
     */
    [[nodiscard]] SharedWorkflowReportNodes getChildren() const;

    /**
     * @brief Returns whether this subtree contains errors.
     * @return True if this node or any descendant contains an error.
     */
    [[nodiscard]] bool hasErrorsRecursive() const;

    /**
     * @brief Counts warnings in this subtree.
     * @return Number of warning messages in this node and its descendants.
     */
    [[nodiscard]] std::int32_t getWarningCountRecursive() const;
    
    /**
     * @brief Counts errors in this subtree.
     * @return Number of error messages in this node and its descendants.
     */
    [[nodiscard]] std::int32_t getErrorCountRecursive() const;

private:

    QString                     _name;          /**< Human-readable report node name. */
    mutable QMutex              _mutex;         /**< Protects messages and child nodes. */
    WorkflowMessages            _messages;      /**< Messages emitted for this report node. */
    SharedWorkflowReportNodes   _children;      /**< Child report nodes. */
};

}
