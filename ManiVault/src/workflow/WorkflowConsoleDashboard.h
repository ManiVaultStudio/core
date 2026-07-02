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

/**
 * @brief Console dashboard for live workflow progress reporting.
 *
 * Periodically renders the workflow progress tree to the console from a
 * background thread. The dashboard observes a shared workflow execution state
 * and formats its current progress tree whenever `render()` is called.
 *
 * The dashboard is started explicitly with `start()` and stopped with `stop()`.
 * Destruction also stops the background thread if it is still running.
 *
 * @note This class is currently in a work-in-progress state and may change.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowConsoleDashboard final
{
public:

    /**
     * @brief Constructs a console dashboard for a workflow execution state.
     *
     * @param state Shared workflow execution state to observe.
     */
    explicit WorkflowConsoleDashboard(WorkflowExecutionState::Ptr state);

    /** Stops the dashboard thread. */
    ~WorkflowConsoleDashboard();

    /**
     * @brief Starts periodic console rendering on a background thread.
     *
     * @note Does nothing if the dashboard is already running.
     */
    void start();

    /**
    * @brief Stops periodic rendering and joins the background thread.
    *
    * @note Wakes the dashboard thread if it is currently waiting between renders.
    */
    void stop();

    /** Renders the current workflow progress tree to the console. */
    void render() const;

private:

    /**
     * @brief Runs the background render loop.
     *
     * Repeatedly renders the dashboard while running, waiting on a condition
     * variable between renders to avoid busy-spinning.
     */
    void run();

private:
    WorkflowExecutionState::Ptr     _state;             /**< Shared workflow execution state being observed */
    std::atomic_bool                _running = false;   /**< Whether the dashboard thread should continue running */
    std::thread                     _thread;            /**< Background thread used for periodic rendering */
    std::condition_variable         _condition;         /**< Wakes the render loop when the dashboard is stopped */
    std::mutex                      _conditionMutex;    /**< Mutex used with _condition */
};


}
