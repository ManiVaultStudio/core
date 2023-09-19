// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractTaskTester.h"

#include "ModalTask.h"

namespace hdps
{

class ModalTaskTester final : public AbstractTaskTester
{
    Q_OBJECT

public:
    Q_INVOKABLE ModalTaskTester(QObject* parent, const QString& name);

private:
    void testRunningIndeterminate();
    void testAggregation();
    void testPerformance();
};

}

Q_DECLARE_METATYPE(hdps::ModalTaskTester)

inline const auto modalTaskTesterMetaTypeId = qRegisterMetaType<hdps::ModalTaskTester*>("hdps::ModalTaskTester");
