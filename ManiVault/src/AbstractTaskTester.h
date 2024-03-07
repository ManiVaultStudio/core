// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Task.h"

#include <QObject>
#include <QStringList>

namespace mv
{

class AbstractTaskTester : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE AbstractTaskTester(QObject* parent, const QString& name);

    virtual void run() {};

    static void registerTester(const QString taskTesterName);

    static QStringList getTesterNames();

private:
    QString     _name;

protected:
    static QStringList testerNames;
};

}