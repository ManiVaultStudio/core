// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ModalTaskTester.h"

class ModalTaskTestRunningIndeterminate : public ModalTaskTester
{
    Q_OBJECT

public:
    Q_INVOKABLE ModalTaskTestRunningIndeterminate(QObject* parent, const QString& name);

    void run() override;

private:
    QStringList _tasks;
};

Q_DECLARE_METATYPE(ModalTaskTestRunningIndeterminate)

inline const auto modalTaskTestRunningIndeterminateMetaTypeId = qRegisterMetaType<ModalTaskTestRunningIndeterminate*>("ModalTaskTestRunningIndeterminate");