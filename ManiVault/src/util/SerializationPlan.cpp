// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "SerializationPlan.h"

mv::util::SerializationPlan::Job::Job(QString name, JobFunction function) :
    _name(std::move(name)),
    _function(std::move(function))
{
}

mv::util::SerializationPlan::Stage::Stage(QString name, Jobs jobs) :
    _name(std::move(name)),
    _jobs(std::move(jobs))
{
}

void mv::util::SerializationPlan::addStage(Stage stage)
{
    _stages.emplace_back(std::move(stage));
}

void mv::util::SerializationPlan::addSequentialStage(QString name, JobFunction jobFunction)
{
    _stages.emplace_back(Stage(name, {
        Job(name, std::move(jobFunction))
    }));
}

void mv::util::SerializationPlan::addParallelStage(QString name, Jobs jobs)
{
    _stages.emplace_back(Stage(std::move(name), std::move(jobs)));
}

void mv::util::SerializationPlan::execute(AbstractSerializationPlanExecutor& serializationPlanExecutor)
{
    serializationPlanExecutor.execute(*this);
}

