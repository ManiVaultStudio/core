// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowConsoleDashboard.h"
#include "WorkflowConsoleFormatter.h"

#include <iostream>

namespace mv::util
{

WorkflowConsoleDashboard::WorkflowConsoleDashboard(WorkflowExecutionState::Ptr state) :
    _state(std::move(state))
{
}

void WorkflowConsoleDashboard::render()
{
    if (!_state)
        return;

    auto root = _state->getProgressRoot();

    if (!root)
        return;

    const auto snapshot = root->createSnapshot();
    const auto text     = WorkflowConsoleFormatter::formatProgressTree(snapshot);

    // Clear screen
    std::cout << "\x1b[2J";

    // Move cursor to top-left
    std::cout << "\x1b[H";
    std::cout << text.toStdString();

    std::cout.flush();
}

}
