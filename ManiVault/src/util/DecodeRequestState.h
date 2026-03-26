// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include <util/Serialization.h>

#include <QObject>

class DecodeRequestState final : public QObject
{
    Q_OBJECT

public:
    explicit DecodeRequestState(QObject* parent = nullptr);

    bool isFinished() const;
    bool hasError() const;
    QString errorString() const;
    int totalJobs() const;
    int completedJobs() const;

signals:
    void progressChanged(int completed, int total);
    void finished();
    void failed(const QString& error);

private:
    mutable QMutex  _mutex;
    bool            _finished = false;
    QString         _error;
    int             _totalJobs = 0;
    int             _completedJobs = 0;

    QFutureWatcher<QVector<mv::util::DecodeBlockResult>> _watcher;

    friend class SerializationScheduler;
};

using DecodeRequestHandle = QSharedPointer<DecodeRequestState>;