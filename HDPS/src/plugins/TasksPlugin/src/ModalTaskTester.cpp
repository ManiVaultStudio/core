// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ModalTaskTester.h"

using namespace hdps;

ModalTaskTester::ModalTaskTester(QObject* parent, const QString& name) :
    AbstractTaskTester(parent, name),
    _modalTask(parent, name)
{
}

void ModalTaskTester::run()
{
}

hdps::Task* ModalTaskTester::getTask()
{
    return &_modalTask;
}
