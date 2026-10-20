// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowConsoleDashboard.h"
#include "WorkflowConsoleFormatter.h"

#include <QMutexLocker>

#include <chrono>
#include <iostream>

namespace mv::workflow
{

QMutex& workflowConsoleMutex()
{
    static QMutex mutex;
    return mutex;
}


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

    _condition.notify_all();

    if (_thread.joinable())
        _thread.join();

    render();

    {
        QMutexLocker lock(&workflowConsoleMutex());
        std::cout << std::endl;
    }
}

void WorkflowConsoleDashboard::run()
{
    using namespace std::chrono_literals;

    while (_running) {
        render();

        std::unique_lock lock(_conditionMutex);

        _condition.wait_for(lock, 100ms, [this] {
            return !_running.load();
            });
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

    QMutexLocker lock(&workflowConsoleMutex());

    std::cout << "\x1b[2J";
    std::cout << "\x1b[H";
    std::cout << text.toStdString();
    std::cout.flush();
}

}
