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
 * @brief The WorkflowReportNode class represents a node in a hierarchical workflow report structure.
 *
 * Each node can contain messages and child nodes, allowing for a structured representation of workflow execution results, warnings, and errors.
 *
 * @author T. Kroes BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowReportNode
{
public:

    Q_DISABLE_COPY_MOVE(WorkflowReportNode)

    /** A shared pointer type for WorkflowReportNode, allowing for easy management of node lifetimes and shared ownership across different parts of the workflow reporting system. */
    using SharedWorkflowReportNode = std::shared_ptr<WorkflowReportNode>;

    /** A vector of shared pointers to WorkflowReportNode, representing the children of a report node in the hierarchical structure. */
    using SharedWorkflowReportNodes = std::vector<SharedWorkflowReportNode>;

    /**
     * @brief Constructs a WorkflowReportNode with the given name.
     * @param name The name of the report node, typically representing a specific step or component in the workflow.
     */
    explicit WorkflowReportNode(QString name);

    /**
     * @brief Creates a child report node with the specified name and adds it to this node's children.
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
     * @brief Checks if this report node or any of its descendants contain any error messages.
     * @return True if there are error messages in this node or any of its descendants, false otherwise.
     */
    bool hasErrorsRecursive() const;

    /**
     * @brief Gets the total count of warning messages in this report node and all of its descendants.
     * @return The total number of warning messages found in this node and its descendants.
     */
    std::int32_t getWarningCountRecursive() const;
    
    /**
     * @brief Get the total count of error messages in this report node and all of its descendants.
     * @return The total number of error messages found in this node and its descendants.
     */
    std::int32_t getErrorCountRecursive() const;

private:
    QString                     _name;          /** The name of the report node, typically representing a specific step or component in the workflow */
    mutable QMutex              _mutex;         /** Mutex to protect access to the messages and children vectors for thread safety */
    WorkflowMessages            _messages;      /** A list of messages associated with this report node, including informational messages, warnings, and errors */
    SharedWorkflowReportNodes   _children;      /** A list of child report nodes, allowing for a hierarchical structure to represent sub-steps or components within the workflow */
};

} // namespace mv::util