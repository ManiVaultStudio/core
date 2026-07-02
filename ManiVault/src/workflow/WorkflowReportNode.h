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
 * @brief Stores messages for one node in a hierarchical workflow report.
 *
 * WorkflowReportNode mirrors the logical workflow execution hierarchy. Each
 * node owns the messages emitted for one workflow, stage, job, or nested
 * execution scope, and can contain child report nodes for subordinate scopes.
 * This allows callers to inspect messages locally or aggregate diagnostics
 * recursively across the whole report tree.
 *
 * The node protects access to its name, messages, and children internally so
 * workflow execution code can append messages and create child report nodes from
 * multiple execution paths.
 *
 * @maintainer T. Kroes BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowReportNode
{
public:

    Q_DISABLE_COPY_MOVE(WorkflowReportNode)

    /** Shared ownership pointer type used for report nodes. */
    using SharedWorkflowReportNode = std::shared_ptr<WorkflowReportNode>;

    /** Ordered collection of child report nodes. */
    using SharedWorkflowReportNodes = std::vector<SharedWorkflowReportNode>;

    /**
     * @brief Constructs a report node.
     * @param name Human-readable report node name.
     */
    explicit WorkflowReportNode(QString name);

    /**
     * @brief Creates and registers a child report node.
     * @param name Human-readable child report node name.
     * @return Shared pointer to the newly created child report node.
     */
    SharedWorkflowReportNode createChild(const QString& name);

    /**
     * @brief Adds a timestamped message to this report node.
     *
     * Messages are stored in insertion order. The timestamp is captured in UTC
     * when the message is added.
     *
     * @param level Severity level of the message.
     * @param emitter Component, workflow, or subsystem that emitted the message.
     * @param text Human-readable message text.
     * @param location Optional source or workflow location associated with the message.
     * @param details Optional structured metadata associated with the message.
     */
    void addMessage(util::SeverityLevel level, const QString& emitter, const QString& text, const QString& location, const QVariantMap& details = {});

    /**
     * @brief Returns the human-readable report node name.
     * @return Report node name.
     */
    QString getName() const;

    /**
     * @brief Returns the messages stored directly on this node.
     * @return Copy of this node's messages.
     */
    WorkflowMessages getMessages() const;

    /**
     * @brief Returns direct child report nodes.
     * @return Copy of this node's child pointer list.
     */
    SharedWorkflowReportNodes getChildren() const;

    /**
     * @brief Returns whether this node or any descendant contains an error.
     * @return True if at least one recursive message has Error severity.
     */
    bool hasErrorsRecursive() const;

    /**
     * @brief Counts warning messages on this node and all descendants.
     * @return Recursive warning message count.
     */
    std::int32_t getWarningCountRecursive() const;

    /**
     * @brief Counts error messages on this node and all descendants.
     * @return Recursive error message count.
     */
    std::int32_t getErrorCountRecursive() const;

private:
    QString                     _name;          /**< Human-readable report node name */
    mutable QMutex              _mutex;         /**< Protects access to the node name, messages, and children */
    WorkflowMessages            _messages;      /**< Messages emitted directly for this report node */
    SharedWorkflowReportNodes   _children;      /**< Direct child nodes for nested workflow report scopes */
};

}
