// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowConsoleDashboard.h"
#include "WorkflowConsoleFormatter.h"

#include <chrono>
#include <iostream>

namespace mv::util
{

WorkflowConsoleDashboard::WorkflowConsoleDashboard(WorkflowExecutionState::Ptr state) :
    _state(std::move(state))
{
}

WorkflowConsoleDashboard::~WorkflowConsoleDashboard()
{
    stop();
}

void WorkflowConsoleDashboard::start()
{
    if (_running.exchange(true))
        return;

    _thread = std::thread([this] {
    	run();
    });
}

void WorkflowConsoleDashboard::stop()
{
    if (!_running.exchange(false))
        return;

    if (_thread.joinable())
        _thread.join();

    render();

    std::cout << std::endl;
}

void WorkflowConsoleDashboard::run() const
{
    using namespace std::chrono_literals;

    while (_running) {
        render();
        std::this_thread::sleep_for(250ms);
    }
}

void WorkflowConsoleDashboard::render() const
{
    if (!_state)
        return;

    const auto root = _state->getProgressRoot();

    if (!root)
        return;

    const auto snapshot = root->createSnapshot();
    const auto text = WorkflowConsoleFormatter::formatProgressTree(snapshot);

    std::cout << "\x1b[2J";
    std::cout << "\x1b[H";
    std::cout << text.toStdString();
    std::cout.flush();
}

}
