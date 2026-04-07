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

const SerializationPlan::JobFunction& SerializationPlan::Job::getFunction() const
{
	return _function;
}

void SerializationPlan::Job::run()
{
    clearError();
    clearResult();

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

void SerializationPlan::Job::clearResult()
{
	_result.clear();
}

void SerializationPlan::Job::setError(QString error)
{
	_error = std::move(error);
}

bool SerializationPlan::Job::hasError() const
{
	return _error.has_value();
}

const QString& SerializationPlan::Job::getError() const
{
	return *_error;
}

void SerializationPlan::Job::clearError()
{
	_error.reset();
}

void SerializationPlan::Job::setException(const std::exception& e)
{
	_error = QString::fromUtf8(e.what());
}

void SerializationPlan::Job::setUnknownException()
{
	_error = QStringLiteral("Unknown exception");
}

void SerializationPlan::Job::fail(QString error)
{
	setError(std::move(error));
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

void SerializationPlan::addParallelStage(QString name, Jobs jobs)
{
    _stages.emplace_back(std::move(name), Stage::Mode::Parallel, std::move(jobs));
}

void SerializationPlan::execute(AbstractSerializationPlanExecutor& serializationPlanExecutor)
{
    serializationPlanExecutor.execute(*this);
}

}