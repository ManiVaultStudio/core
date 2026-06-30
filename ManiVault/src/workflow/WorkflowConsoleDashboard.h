// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowExecutionState.h"

#include <atomic>
#include <memory>
#include <thread>

namespace mv::workflow
{

class CORE_EXPORT WorkflowConsoleDashboard
{
public:
    explicit WorkflowConsoleDashboard(WorkflowExecutionState::Ptr state);
    ~WorkflowConsoleDashboard();

    void start();
    void stop();

    void render() const;

private:
    void run() const;

private:
    WorkflowExecutionState::Ptr _state;

    std::atomic_bool _running = false;
    std::thread _thread;
};


}