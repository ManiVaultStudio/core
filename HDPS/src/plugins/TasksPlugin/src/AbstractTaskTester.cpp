// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AbstractTaskTester.h"

using namespace hdps;

QStringList AbstractTaskTester::testerNames = QStringList();

AbstractTaskTester::AbstractTaskTester(QObject* parent, const QString& name) :
    QObject(parent),
    _name(name)
{
}

void AbstractTaskTester::registerTester(const QString taskTesterName)
{
    AbstractTaskTester::testerNames << taskTesterName;
}

QStringList AbstractTaskTester::getTesterNames()
{
    return testerNames;
}
