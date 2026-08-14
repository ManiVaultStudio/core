// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "Parallel.h"

#include "Application.h"

#include <atomic>
#include <stdexcept>
#include <vector>

namespace mv
{

ParallelExecutionChain Parallel::stages(const QString& name)
{
    return ParallelExecutionChain(name);
}

workflow::SharedWorkflowResult Parallel::runPhantomTest(const workflow::WorkflowOptions& options)
{
    std::atomic_int counter = 0;
    auto setupOptions       = options;

    setupOptions.reporting.finishedNotification = false;

    run("Parallel phantom run", [&counter]() {
        counter.fetch_add(1);
    }, setupOptions);

    forEach("Parallel phantom forEach", std::vector<int>{ 1, 2, 3, 4 }, [&counter](int value) {
        counter.fetch_add(value);
    }, setupOptions);

    const auto squares = map("Parallel phantom map", std::vector<int>{ 1, 2, 3, 4 }, [](int value) {
        return value * value;
    }, setupOptions);

    return stages("Parallel phantom staged")
        .run("Read phantom inputs", [&counter]() {
            counter.fetch_add(1);
        })
        .forEach("Process phantom inputs", squares, [&counter](int value) {
            counter.fetch_add(value);
        })
        .run("Finalize phantom inputs", [&counter]() {
            counter.fetch_add(1);

            if (counter.load() != 43)
                throw std::runtime_error("Parallel phantom test produced an unexpected result");
        })
        .execute(options);
}

workflow::SharedWorkflowResult Parallel::executePlan(workflow::UniqueWorkflowPlan plan, const workflow::WorkflowOptions& options)
{
    return Application::getWorkflowPlanExecutor().executeBlocking(std::move(plan), nullptr, options);
}

}
