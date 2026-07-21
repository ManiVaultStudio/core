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
 * @maintainer T. Kroes BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)
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
     * @brief Constructs a WorkflowReportNode with the given name.
     * @param name The name of the report node, typically representing a specific step or component in the workflow.
     */
    explicit WorkflowReportNode(QString name);

    /**
     * @brief Creates a child report node.
     *
     * @param name The name of the child report node.
     * @return A shared pointer to the newly created child report node.
     */
    SharedWorkflowReportNode createChild(const QString& name);

    /**
     * Adds a message to this report node.
     * @param level The severity level of the message (e.g., Info, Warning, Error, Fatal).
     * @param emitter The name of the component or module that generated the message.
     * @param text The main text or content of the message.
     * @param location The specific location in the code or workflow where the message was generated (e.g., function name, line number).
     * @param details Additional details or metadata associated with the message (optional).
     */
    void addMessage(util::SeverityLevel level, const QString& emitter, const QString& text, const QString& location, const QVariantMap& details = {});

    /**
     * @brief Retrieves the name of this report node.
     * @return The name of the report node.
     */
    QString getName() const;

    /**
     * @brief Retrieves the messages associated with this report node.
     * @return A vector of WorkflowMessage objects representing the messages for this node.
     */
    WorkflowMessages getMessages() const;

    /**
     * @brief Retrieves the child report nodes of this report node.
     * @return A vector of shared pointers to WorkflowReportNode representing the children of this node.
     */
    SharedWorkflowReportNodes getChildren() const;

    /**
     * @brief Checks for recursive errors.
     *
     * @return True if there are error messages in this node or any of its descendants, false otherwise.
     */
    bool hasErrorsRecursive() const;

    /**
     * @brief Gets the recursive warning count.
     *
     * @return The total number of warning messages found in this node and its descendants.
     */
    std::int32_t getWarningCountRecursive() const;
    
    /**
     * @brief Gets the recursive error count.
     *
     * @return The total number of error messages found in this node and its descendants.
     */
    std::int32_t getErrorCountRecursive() const;

private:

    QString                     _name;          /**< Human-readable report node name */
    mutable QMutex              _mutex;         /**< Protects messages and child nodes */
    WorkflowMessages            _messages;      /**< Messages emitted for this report node */
    SharedWorkflowReportNodes   _children;      /**< Child report nodes */
};

}
