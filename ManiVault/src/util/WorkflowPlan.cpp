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

WorkflowPlan::Job::Job(QString name, JobFunction function, JobThreadAffinity threadAffinity /*= JobThreadAffinity::CurrentWorkerThread*/) :
    _name(std::move(name)),
    _function(std::move(function)),
    _threadAffinity(threadAffinity)
{
}

WorkflowPlan::Job::Job(QString name, JobFunction function, double weight) :
    _name(std::move(name)),
    _function(std::move(function)),
    _weight(weight)
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

void WorkflowPlan::Job::run() const
{
//#ifdef WORKFLOW_PLAN_VERBOSE
//    qDebug() << "Running job:" << _name;
//#endif

    if (_function)
		_function(*const_cast<WorkflowPlan::Job*>(this));
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

WorkflowPlan::JobThreadAffinity WorkflowPlan::Job::getThreadAffinity() const
{
	return _threadAffinity;
}

double WorkflowPlan::Job::getWeight() const
{
	return _weight;
}

void WorkflowPlan::Stage::setWeight(double weight)
{
	_weight = weight;
}

double WorkflowPlan::Stage::getWeight() const
{
	return _weight;
}

bool WorkflowPlan::Stage::containsGuiThreadJobs() const
{
	for (const auto& job : _jobs) {
		if (job.getThreadAffinity() == WorkflowPlan::JobThreadAffinity::GuiThread)
			return true;
	}

	return false;
}

bool WorkflowPlan::Stage::containsWorkerThreadJobs() const
{
	for (const auto& job : _jobs) {
		if (job.getThreadAffinity() == WorkflowPlan::JobThreadAffinity::CurrentWorkerThread)
			return true;
	}

	return false;
}

bool WorkflowPlan::Stage::isThreadAffinityCompatible() const
{
	if (_concurrencyMode == ConcurrencyMode::Sequential)
		return true;

	if (_concurrencyMode == ConcurrencyMode::Parallel && containsGuiThreadJobs())
		return false;

	return true;
}

QString WorkflowPlan::getName() const
{
	return _name;
}

WorkflowPlan::Stages WorkflowPlan::getStages() const
{
	return _stages;
}

WorkflowPlan::SharedState WorkflowPlan::getSharedState() const
{
	return _sharedState;
}

SharedWorkflowContext WorkflowPlan::getWorkflowContext() const
{
	return _workflowContext;
}

void WorkflowPlan::setWeight(double weight)
{
	_weight = weight;
}

double WorkflowPlan::getWeight() const
{
	return _weight;
}

WorkflowPlan::Stage::Stage(QString name, ConcurrencyMode concurrencyMode, Jobs jobs, double weight /*= 1.0*/) :
    _name(std::move(name)),
    _concurrencyMode(concurrencyMode),
    _jobs(std::move(jobs)),
    _weight(weight)
{
}

WorkflowPlan::ConcurrencyMode WorkflowPlan::Stage::getConcurrencyMode() const
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
    _name(title),
    _workflowContext(std::move(context))
{
}

void WorkflowPlan::addStage(Stage stage)
{
    _stages.emplace_back(std::move(stage));
}

void WorkflowPlan::addSequentialStage(QString name, Jobs jobs, double weight /*= 1.0*/)
{
    if (jobs.empty()) {
        qWarning() << "Attempted to add empty sequential stage:" << name;
	    return;
    }
        
    _stages.emplace_back(std::move(name), ConcurrencyMode::Sequential, std::move(jobs), weight);
}

void WorkflowPlan::addParallelStage(QString name, Jobs jobs, double weight /*= 1.0*/)
{
    if (jobs.empty()) {
        qWarning() << "Attempted to add empty parallel stage:" << name;
        return;
    }

    _stages.emplace_back(std::move(name), ConcurrencyMode::Parallel, std::move(jobs), weight);
}

void WorkflowPlan::addStage(QString name, ConcurrencyMode mode, Jobs jobs, double weight /*= 1.0*/)
{
	switch (mode) {
		case ConcurrencyMode::Sequential:
            addSequentialStage(std::move(name), std::move(jobs), weight);
            break;

        case ConcurrencyMode::Parallel:
            addParallelStage(std::move(name), std::move(jobs), weight);
            break;
	}
}

WorkflowResult WorkflowPlan::execute(const SharedWorkflowPlanExecutor& workflowPlanExecutor, bool showProgress, WorkflowExecutionOptions executionOptions /*= {}*/)
{
    if (!workflowPlanExecutor) {
        qWarning() << "Unable to execute workflow plan: no executor provided!";
        return {};
    }

    return workflowPlanExecutor->execute(*this, showProgress, executionOptions);
}

WorkflowResultFuture WorkflowPlan::executeAsync(const SharedWorkflowPlanExecutor& workflowPlanExecutor, bool showProgress, WorkflowExecutionOptions executionOptions /*= {}*/)
{
    if (!workflowPlanExecutor) {
        qWarning() << "Unable to execute workflow plan: no executor provided!";
        return WorkflowResultFuture::makeReady(WorkflowResult());
    }

    return workflowPlanExecutor->executeAsync(*this, showProgress, executionOptions);
}

WorkflowResult WorkflowPlan::executeOnCurrentThread(const SharedWorkflowPlanExecutor& workflowPlanExecutor, Task* task, WorkflowExecutionOptions executionOptions)
{
    if (!workflowPlanExecutor) {
        qWarning() << "Unable to execute workflow plan: no executor provided!";
        return{};
    }

    return workflowPlanExecutor->executeOnCurrentThread(*this, task, executionOptions);
}

}
