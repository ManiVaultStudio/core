// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "ParallelExecutionChain.h"

#include "Application.h"

#include <stdexcept>

namespace mv
{

ParallelExecutionChain::ParallelExecutionChain(QString name) :
    _name(std::move(name))
{
}

ParallelExecutionChain& ParallelExecutionChain::then(ParallelExecutionChain other) &
{
    _stages.reserve(_stages.size() + other._stages.size());

    for (auto& stage : other._stages)
        _stages.push_back(std::move(stage));

    other._stages.clear();

    return *this;
}

ParallelExecutionChain&& ParallelExecutionChain::then(ParallelExecutionChain other) &&
{
    then(std::move(other));

    return std::move(*this);
}

workflow::SharedWorkflowResult ParallelExecutionChain::execute(const workflow::WorkflowOptions& options) &&
{
    if (_executed)
        throw std::logic_error("Parallel execution chain has already been executed");

    _executed = true;

    auto plan = std::make_unique<workflow::WorkflowPlan>(_name.isEmpty() ? QStringLiteral("Parallel execution") : _name);

    for (auto& stage : _stages)
        stage.appendTo(*plan);

    return Application::getWorkflowPlanExecutor().executeBlocking(std::move(plan), nullptr, options);
}

void ParallelExecutionChain::addStage(QString name, AppendFunction appendTo)
{
    _stages.push_back(Stage{ std::move(name), std::move(appendTo) });
}

}
