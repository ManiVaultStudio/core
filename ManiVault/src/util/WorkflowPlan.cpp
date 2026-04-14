// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "WorkflowPlan.h"

#ifdef _DEBUG
	#define WORKFLOW_PLAN_VERBOSE
#endif

#define WORKFLOW_PLAN_VERBOSE

namespace mv::util
{

WorkflowPlan::Job::Job(QString name, JobFunction function) :
    _name(std::move(name)),
    _function(std::move(function))
{
}

QString WorkflowPlan::Job::getName() const
{
	return _name;
}

const WorkflowPlan::JobFunction& WorkflowPlan::Job::getFunction() const
{
	return _function;
}

void WorkflowPlan::Job::run()
{
#ifdef WORKFLOW_PLAN_VERBOSE
    qDebug() << "Running job:" << _name;
#endif

    clearError();
    clearResult();

    if (_function)
		_function(*this);
}

void WorkflowPlan::Job::setResult(QVariant result)
{
	_result = std::move(result);
}

const QVariant& WorkflowPlan::Job::getResult() const
{
	return _result;
}

void WorkflowPlan::Job::clearResult()
{
	_result.clear();
}

void WorkflowPlan::Job::setError(QString error)
{
	_error = std::move(error);
}

bool WorkflowPlan::Job::hasError() const
{
	return _error.has_value();
}

const QString& WorkflowPlan::Job::getError() const
{
	return *_error;
}

void WorkflowPlan::Job::clearError()
{
	_error.reset();
}

void WorkflowPlan::Job::setException(const std::exception& e)
{
	_error = QString::fromUtf8(e.what());
}

void WorkflowPlan::Job::setUnknownException()
{
	_error = QStringLiteral("Unknown exception");
}

void WorkflowPlan::Job::fail(QString error)
{
	setError(std::move(error));
}

WorkflowPlan::Stage::Stage(QString name, ConcurrencyMode concurrencyMode, Jobs jobs) :
    _name(std::move(name)),
    _concurrencyMode(concurrencyMode),
    _jobs(std::move(jobs))
{
}

WorkflowPlan::ConcurrencyMode WorkflowPlan::Stage::getMode() const
{
	return _concurrencyMode;
}

QString WorkflowPlan::Stage::getName() const
{
	return _name;
}

WorkflowPlan::Jobs WorkflowPlan::Stage::getJobs() const
{
	return _jobs;
}

WorkflowPlan::WorkflowPlan(const QString& title, SharedWorkflowContext context) :
    _title(title),
    _workflowContext(std::move(context))
{
}

void WorkflowPlan::addStage(Stage stage)
{
    _stages.emplace_back(std::move(stage));
}

void WorkflowPlan::addSequentialStage(QString name, Jobs jobs)
{
    if (jobs.empty()) {
        qWarning() << "Attempted to add empty sequential stage:" << name;
	    return;
    }
        
    _stages.emplace_back(std::move(name), ConcurrencyMode::Sequential, std::move(jobs));
}

void WorkflowPlan::addParallelStage(QString name, Jobs jobs)
{
    if (jobs.empty()) {
        qWarning() << "Attempted to add empty parallel stage:" << name;
        return;
    }

    _stages.emplace_back(std::move(name), ConcurrencyMode::Parallel, std::move(jobs));
}

void WorkflowPlan::addStage(QString name, ConcurrencyMode mode, Jobs jobs)
{
	switch (mode) {
		case ConcurrencyMode::Sequential:
            addSequentialStage(std::move(name), std::move(jobs));
            break;

        case ConcurrencyMode::Parallel:
            addParallelStage(std::move(name), std::move(jobs));
            break;
	}
}

void WorkflowPlan::execute(AbstractWorkflowPlanExecutor& WorkflowPlanExecutor)
{
    WorkflowPlanExecutor.execute(*this);
}

}