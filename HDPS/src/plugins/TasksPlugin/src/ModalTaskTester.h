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