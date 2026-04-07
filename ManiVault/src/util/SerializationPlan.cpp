// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "SerializationPlan.h"

namespace mv::util
{

SerializationPlan::Job::Job(QString name, JobFunction function) :
    _name(std::move(name)),
    _function(std::move(function))
{
}

QString SerializationPlan::Job::getName() const
{
	return _name;
}

SerializationPlan::JobFunction SerializationPlan::Job::getFunction() const
{
	return _function;
}

void SerializationPlan::Job::run()
{
	qDebug() << "Running job:" << _name;
	if (_function)
		_function(*this);
}

void SerializationPlan::Job::setResult(QVariant result)
{
	_result = std::move(result);
}

const QVariant& SerializationPlan::Job::getResult() const
{
	return _result;
}

SerializationPlan::Stage::Stage(QString name, Mode mode, Jobs jobs) :
    _name(std::move(name)),
    _mode(mode),
    _jobs(std::move(jobs))
{
}

SerializationPlan::Stage::Mode SerializationPlan::Stage::getMode() const
{
	return _mode;
}

QString SerializationPlan::Stage::getName() const
{
	return _name;
}

SerializationPlan::Jobs SerializationPlan::Stage::getJobs() const
{
	return _jobs;
}

void SerializationPlan::addStage(Stage stage)
{
    _stages.emplace_back(std::move(stage));
}

void SerializationPlan::addSequentialStage(QString name, JobFunction jobFunction)
{
    _stages.emplace_back(Stage(name, Stage::Mode::Sequential, {
        Job(name, std::move(jobFunction))
    }));
}

void SerializationPlan::addParallelStage(QString name, Jobs jobs)
{
    _stages.emplace_back(std::move(name), Stage::Mode::Parallel, std::move(jobs));
}

void SerializationPlan::execute(AbstractSerializationPlanExecutor& serializationPlanExecutor)
{
    serializationPlanExecutor.execute(*this);
}

}