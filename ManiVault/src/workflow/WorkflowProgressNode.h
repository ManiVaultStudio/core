// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowExecutionNodeType.h"

#include <QString>
#include <QDateTime>
#include <QMutex>
#include <QVector>
#include <QDebug>

namespace mv::workflow
{

/**
 * @brief Tracks progress for a node in the workflow execution tree.
 *
 * WorkflowProgressNode mirrors the hierarchy created by WorkflowExecutionContext.
 * Leaf nodes store explicit progress values, while parent nodes derive their
 * progress from the weighted average of their children. This allows the root
 * node to represent aggregate progress for the entire workflow execution.
 *
 * The class is designed for concurrent workflow execution: public accessors and
 * mutators synchronize access internally, and createSnapshot() returns a value
 * tree suitable for dashboards, console formatting, and diagnostics.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class WorkflowProgressNode : public std::enable_shared_from_this<WorkflowProgressNode>
{
public:
    using Type = WorkflowExecutionNodeType;

    /** Lifecycle state of a progress node. */
    enum class Status {
        Pending,    /**< Node has been created but has not started. */
        Running,    /**< Node is currently executing. */
        Completed,  /**< Node finished successfully. */
        Failed,     /**< Node finished with an error. */
        Skipped     /**< Node was intentionally skipped. */
    };

    /**
     * @brief Value copy of a progress node and its descendants.
     *
     * Snapshots decouple rendering and reporting code from the live progress
     * tree. They capture the node's current state, aggregate progress, elapsed
     * time, child counts, and recursively captured child snapshots.
     */
    struct Snapshot
    {
        Type                type = Type::Undefined;         /**< Semantic workflow node type. */
        Status              status = Status::Pending;       /**< Current lifecycle state. */
        QString             name;                           /**< Human-readable node name. */
        double              progress = 0.0;                 /**< Normalized progress in the range [0.0, 1.0]. */
        double              weight = 1.0;                   /**< Relative contribution to the parent node. */
        int                 childCount = 0;                 /**< Number of direct child nodes. */
        int                 completedChildCount = 0;        /**< Number of direct children with a terminal status. */
        qint64              elapsedMilliseconds = 0;        /**< Elapsed runtime in milliseconds. */
        QVector<Snapshot>   children;                       /**< Snapshots of direct child nodes. */
    };

    /**
     * @brief Creates a recursive snapshot of this progress subtree.
     * @return Snapshot containing the current node state and child snapshots.
     */
    Snapshot createSnapshot() const;

public:

    /** Shared pointer type used for progress node ownership. */
    using Ptr = std::shared_ptr<WorkflowProgressNode>;

    /**
     * @brief Constructs a progress node.
     *
     * Prefer createRoot() and createChild() in workflow code so parent-child
     * relationships are established consistently.
     *
     * @param type Semantic workflow node type.
     * @param name Human-readable node name.
     * @param parent Parent progress node, or nullptr for a root node.
     * @param weight Relative contribution to the parent node's progress.
     */
    explicit WorkflowProgressNode(Type type, QString name, Ptr parent, double weight = 1.0);

    /**
     * @brief Creates a root progress node.
     * @param type Semantic workflow node type.
     * @param name Human-readable root node name.
     * @param weight Relative progress weight, normally 1.0 for roots.
     * @return Shared root progress node.
     */
    static Ptr createRoot(Type type, const QString& name, double weight = 1.0);

    /**
     * @brief Creates and registers a child progress node.
     * @param type Semantic workflow node type.
     * @param name Human-readable child node name.
     * @param weight Relative contribution to this node's aggregate progress.
     * @return Shared child progress node.
     */
    Ptr createChild(Type type, QString name, double weight = 1.0);

    /**
     * @brief Returns whether this node has direct children.
     * @return True if this node has at least one child.
     */
    bool hasChildren() const;

    /**
     * @brief Sets explicit progress for a leaf node.
     *
     * The value is clamped to [0.0, 1.0]. Calls on non-leaf nodes are ignored
     * because parent progress is computed from child progress.
     *
     * @param value Normalized progress value.
     */
    void setProgress(double value);

    /**
     * @brief Returns this node's current progress.
     *
     * Leaf nodes return their explicit progress. Parent nodes return the
     * weighted average of their children's progress.
     *
     * @return Normalized progress value in the range [0.0, 1.0].
     */
    double getProgress() const;

    /**
     * @brief Returns this node's relative contribution to its parent.
     * @return Progress weight.
     */
    double getWeight() const;

    /**
     * @brief Returns the semantic workflow node type.
     * @return Workflow execution node type.
     */
    Type getType() const;

    /**
     * @brief Returns the human-readable node name.
     * @return Node name.
     */
    QString getName() const;

    /**
     * @brief Returns the current lifecycle state.
     * @return Node status.
     */
    Status getStatus() const;

    /**
     * @brief Returns the parent node.
     * @return Shared parent node, or nullptr for a root node.
     */
    Ptr getParent() const;

    /**
     * @brief Returns direct child nodes.
     * @return Copy of the current child pointer list.
     */
    QVector<Ptr> getChildren() const;

    /**
     * @brief Returns the number of direct child nodes.
     * @return Child count.
     */
    int getChildCount() const;

    /**
     * @brief Returns the number of direct children with terminal status.
     *
     * Completed, failed, and skipped children are all considered finished.
     *
     * @return Finished child count.
     */
    int getCompletedChildCount() const;

    /**
     * @brief Returns elapsed runtime for this node.
     *
     * Pending nodes report zero. Running nodes report the active timer value.
     * Terminal nodes report the elapsed time captured when they finished.
     *
     * @return Elapsed time in milliseconds.
     */
    std::int64_t getElapsedMilliseconds() const;

    /**
     * @brief Returns elapsed runtime without locking.
     *
     * This helper is intended for internal use by callers that already hold
     * this node's mutex.
     *
     * @return Elapsed time in milliseconds.
     */
    std::int64_t getElapsedMillisecondsUnlocked() const;

    /** Marks the node as running and starts elapsed-time tracking if needed. */
    void markRunning();

    /** Marks the node as completed and captures elapsed time if it was running. */
    void markCompleted();

    /** Marks the node as failed and captures elapsed time if it was running. */
    void markFailed();

    /** Marks the node as skipped and captures elapsed time if it was running. */
    void markSkipped();

    /**
     * @brief Returns whether this node has no parent.
     * @return True if this node is a root progress node.
     */
    bool isRoot() const;

private:
    /** Updates lifecycle state and timer bookkeeping. */
    void setStatus(Status status);

    /** Returns whether this node is completed, failed, or skipped. */
    bool isFinished() const;

    /** Returns whether this node is completed, failed, or skipped without locking. */
    bool isFinishedUnlocked() const;

private:
    mutable QMutex                          _mutex;                    /**< Protects access to mutable node state for concurrent workflow updates */
    Type                                    _type = Type::Undefined;   /**< Semantic workflow execution type represented by this node */
    QString                                 _name;                     /**< Human-readable node name used in reports, logs, and progress displays */
    std::weak_ptr<WorkflowProgressNode>     _parent;                   /**< Weak parent reference to avoid ownership cycles in the progress tree */
    double                                  _weight = 1.0;             /**< Relative contribution of this node to its parent's aggregate progress */
    Status                                  _status = Status::Pending; /**< Current lifecycle state of this node */
    double                                  _selfProgress = 0.0;       /**< Explicit progress value used when this node has no children */
    QElapsedTimer                           _timer;                    /**< Timer used to measure elapsed runtime while the node is running */
    qint64                                  _finishedElapsedMs = 0;    /**< Captured elapsed runtime once the node reaches a terminal state */
    QVector<Ptr>                            _children;                 /**< Direct child progress nodes used for hierarchical progress aggregation */
};

}
