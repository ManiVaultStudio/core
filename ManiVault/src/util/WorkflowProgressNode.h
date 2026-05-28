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
#include <QReadWriteLock>

namespace mv::util
{

class WorkflowProgressNode : public std::enable_shared_from_this<WorkflowProgressNode>
{
public:
    using Type = WorkflowExecutionNodeType;

    enum class Status {
        Pending,
        Running,
        Completed,
        Failed,
        Skipped
    };

    struct Snapshot
    {
        Type type = Type::Undefined;
        Status status = Status::Pending;

        QString name;

        double progress = 0.0;
        double weight = 1.0;

        int childCount = 0;
        int completedChildCount = 0;

        qint64 elapsedMilliseconds = 0;

        QVector<Snapshot> children;
    };

    Snapshot createSnapshot() const;

public:
    using Ptr = std::shared_ptr<WorkflowProgressNode>;

    explicit WorkflowProgressNode(Type type, QString name, Ptr parent, double weight = 1.0);

    static Ptr createRoot(Type type, const QString& name, double weight = 1.0);
    Ptr createChild(Type type, QString name, double weight = 1.0);

    bool hasChildren() const;

    void setProgress(double value);

    double getProgress() const;

    double getWeight() const;

    Type getType() const;
    QString getName() const;
    Status getStatus() const;

    Ptr getParent() const;
    QVector<Ptr> getChildren() const;

    int getChildCount() const;
    int getCompletedChildCount() const;

    std::int64_t getElapsedMilliseconds() const;
    std::int64_t getElapsedMillisecondsUnlocked() const;

    void markRunning();
    void markCompleted();
    void markFailed();
    void markSkipped();

    bool isRoot() const;
private:
    void setStatus(Status status);
    bool isFinished() const;
    bool isFinishedUnlocked() const;

private:
    mutable QMutex  _mutex;
    Type            _type = Type::Undefined;
    QString         _name;
    Ptr             _parent;
    double          _weight = 1.0;
    Status          _status = Status::Pending;
    double          _selfProgress = 0.0;
    QElapsedTimer   _timer;
    qint64          _finishedElapsedMs = 0;
    QVector<Ptr>    _children;
};

} // namespace mv::util