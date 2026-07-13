// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowExecutionNodeType.h"

#include <QString>
#include <QElapsedTimer>
#include <QMutex>
#include <QVector>
#include <QDebug>

namespace mv::workflow
{

/**
 * @brief Thread-safe node in the workflow progress tree.
 *
 * Progress nodes mirror the execution hierarchy and aggregate child progress
 * using relative weights. They are shared by execution contexts and the console
 * dashboard.
 */
class WorkflowProgressNode : public std::enable_shared_from_this<WorkflowProgressNode>
{
public:

    /** Semantic execution node type. */
    using Type = WorkflowExecutionNodeType;

    /** Progress lifecycle state. */
    enum class Status {
        Pending,    /**< Node has not started */
        Running,    /**< Node is currently executing */
        Completed,  /**< Node completed successfully */
        Failed,     /**< Node failed */
        Skipped     /**< Node was skipped */
    };

    /** Immutable progress snapshot for rendering and reporting. */
    struct Snapshot
    {
        Type type = Type::Undefined;           /**< Semantic node type */
        Status status = Status::Pending;       /**< Snapshot status */

        QString name;                          /**< Node name */

        double progress = 0.0;                 /**< Aggregated progress value */
        double weight = 1.0;                   /**< Relative progress weight */

        int childCount = 0;                    /**< Number of child nodes */
        int completedChildCount = 0;           /**< Number of completed child nodes */

        qint64 elapsedMilliseconds = 0;        /**< Elapsed execution time in milliseconds */

        QVector<Snapshot> children;            /**< Child snapshots */
    };

    /** @return Thread-safe snapshot of this node and its descendants. */
    Snapshot createSnapshot() const;

public:

    /** Shared pointer type for workflow progress nodes. */
    using Ptr = std::shared_ptr<WorkflowProgressNode>;

    /** Constructs a workflow progress node. */
    explicit WorkflowProgressNode(Type type, QString name, Ptr parent, double weight = 1.0);

    /** Creates a root progress node. */
    static Ptr createRoot(Type type, const QString& name, double weight = 1.0);

    /** Creates a child progress node. */
    Ptr createChild(Type type, QString name, double weight = 1.0);

    /** @return True when this node has child progress nodes. */
    bool hasChildren() const;

    /** Sets direct progress for this node. */
    void setProgress(double value);

    /** @return Aggregated progress for this node. */
    double getProgress() const;

    /** @return Relative progress weight. */
    double getWeight() const;

    /** @return Semantic execution node type. */
    Type getType() const;

    /** @return Human-readable node name. */
    QString getName() const;

    /** @return Current progress status. */
    Status getStatus() const;

    /** @return Parent progress node, or nullptr for root nodes. */
    Ptr getParent() const;

    /** @return Child progress nodes. */
    QVector<Ptr> getChildren() const;

    /** @return Number of child progress nodes. */
    int getChildCount() const;

    /** @return Number of completed child progress nodes. */
    int getCompletedChildCount() const;

    /** @return Elapsed execution time in milliseconds. */
    std::int64_t getElapsedMilliseconds() const;

    /** @return Elapsed execution time without locking. */
    std::int64_t getElapsedMillisecondsUnlocked() const;

    /** Marks this node as running. */
    void markRunning();

    /** Marks this node as completed. */
    void markCompleted();

    /** Marks this node as failed. */
    void markFailed();

    /** Marks this node as skipped. */
    void markSkipped();

    /** @return True when this node has no parent. */
    bool isRoot() const;

private:

    /** Sets the node status. */
    void setStatus(Status status);

    /** @return True when this node has a terminal status. */
    bool isFinished() const;

    /** @return True when this node has a terminal status without locking. */
    bool isFinishedUnlocked() const;

private:

    mutable QMutex                          _mutex;                 /**< Protects progress node state */
    Type                                    _type = Type::Undefined; /**< Semantic execution node type */
    QString                                 _name;                  /**< Human-readable node name */
    std::weak_ptr<WorkflowProgressNode>     _parent;                /**< Parent progress node */
    double                                  _weight = 1.0;          /**< Relative progress weight */
    Status                                  _status = Status::Pending;  /**< Current progress status */
    double                                  _selfProgress = 0.0;    /**< Direct progress for this node */
    QElapsedTimer                           _timer;                 /**< Timer used for elapsed duration */
    qint64                                  _finishedElapsedMs = 0;  /**< Captured elapsed time after finishing */
    QVector<Ptr>                            _children;              /**< Child progress nodes */
};

}
