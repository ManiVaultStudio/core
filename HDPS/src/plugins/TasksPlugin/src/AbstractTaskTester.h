// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QObject>
#include <QStringList>
#include <QTimer>

namespace hdps {
    class Task;
}

class AbstractTaskTester : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE AbstractTaskTester(QObject* parent, const QString& name);

    virtual void run() {};
    virtual hdps::Task* getTask() {
        return nullptr;
    };

    static void registerTester(const QString taskTesterName);

    QTimer& getTimer();

    static QStringList getTesterNames();

private:
    QString     _name;
    QTimer      _timer;

protected:
    static QStringList testerNames;
};