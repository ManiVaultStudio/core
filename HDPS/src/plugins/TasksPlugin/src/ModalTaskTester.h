// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractTaskTester.h"

#include <ModalTask.h>

class ModalTaskTester : public AbstractTaskTester
{
public:
    ModalTaskTester(QObject* parent, const QString& name);

    void run() override;;

    hdps::Task* getTask() override;

private:
    hdps::ModalTask   _modalTask;
};

class ModalTaskTesterSubtasks : public ModalTaskTester
{
    Q_OBJECT

public:
    Q_INVOKABLE ModalTaskTesterSubtasks(QObject* parent, const QString& name);

    void run() override;

private:
    QStringList _tasks;
};

Q_DECLARE_METATYPE(ModalTaskTesterSubtasks)

inline const auto modalTaskTesterSubtasksMetaTypeId = qRegisterMetaType<ModalTaskTesterSubtasks*>("ModalTaskTesterSubtasks");

class ModalTaskTesterAggregate : public ModalTaskTester
{
    Q_OBJECT

public:
    Q_INVOKABLE ModalTaskTesterAggregate(QObject* parent, const QString& name);

    void run() override;

private:
    QVector<TaskRunner*>   _childTaskRunners;
};

Q_DECLARE_METATYPE(ModalTaskTesterAggregate)

inline const auto modalTaskTesterAggregateMetaTypeId = qRegisterMetaType<ModalTaskTesterAggregate*>("ModalTaskTesterAggregate");