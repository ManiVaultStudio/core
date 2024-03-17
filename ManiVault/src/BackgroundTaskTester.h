// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractTaskTester.h"

namespace mv
{

class CORE_EXPORT BackgroundTaskTester final : public AbstractTaskTester
{
    Q_OBJECT

public:
    Q_INVOKABLE BackgroundTaskTester(QObject* parent, const QString& name);
};

}

Q_DECLARE_METATYPE(mv::BackgroundTaskTester)

inline const auto backgroundTaskTesterMetaTypeId = qRegisterMetaType<mv::BackgroundTaskTester*>("mv::BackgroundTaskTester");
