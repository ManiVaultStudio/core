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

WorkflowPlan::Job::Job(QString name, JobFunction function, JobThreadAffinity threadAffinity /*= JobThreadAffinity::CurrentWorkerThread*/, JobProgressMode progressMode /*= JobProgressMode::Automatic*/) :
    _name(std::move(name)),
    _function(std::move(function)),
    _threadAffinity(threadAffinity),
    _progressMode(progressMode)
{
    Q_ASSERT(_result.isValid() || _result.isNull());
}

WorkflowPlan::Job::Job(QString name, NestedWorkflowFunction nestedWorkflowFunction, JobThreadAffinity threadAffinity, JobProgressMode progressMode, double weight):
	_name(std::move(name)),
	_kind(JobKind::NestedWorkflow),
	_nestedWorkflowFunction(std::move(nestedWorkflowFunction)),
	_threadAffinity(threadAffinity),
	_progressMode(progressMode),
	_weight(weight)
{
}

WorkflowPlan::Job::Job(QString name, NestedWorkflowJob job, JobThreadAffinity threadAffinity, double weight):
	_name(std::move(name)),
	_kind(JobKind::NestedWorkflow),
	_nestedWorkflowFunction(std::move(job.function)),
	_threadAffinity(threadAffinity),
	_weight(weight)
{
}

QString WorkflowPlan::Job::getName() const
{
	return _name;
}

void WorkflowPlan::Job::run(SharedWorkflowExecutionContext context) const
{
//#ifdef WORKFLOW_PLAN_VERBOSE
//    qDebug() << "Running job:" << _name;
//#endif

    _function(*this, context);
}

void WorkflowPlan::Job::setResult(QVariant result)
{
	_result = std::move(result);
}

const QVariant& WorkflowPlan::Job::getResult() const
{
    Q_ASSERT(_result.isValid() || _result.isNull());

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

WorkflowPlan::Job WorkflowPlan::Job::weighted(double weight)
{
    _weight = weight;

    return *this;
}

WorkflowPlan::JobProgressMode WorkflowPlan::Job::getProgressMode() const
{
    return _progressMode;
}

SharedWorkflowContext WorkflowPlan::Job::getWorkflowContext() const
{
	return _workflowContext;
}

void WorkflowPlan::Job::setWorkflowContext(SharedWorkflowContext workflowContext)
{
	_workflowContext = std::move(workflowContext);
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

const WorkflowPlan::Stages& WorkflowPlan::getStages() const
{
	return _stages;
}

WorkflowPlan::Stages& WorkflowPlan::getStages()
{
    return _stages;
}

const WorkflowPlan::Stages& WorkflowPlan::getOnSuccessStages() const
{
    return _onSuccessStages;
}

const WorkflowPlan::Stages& WorkflowPlan::getOnFailureStages() const
{
    return _onFailureStages;
}

const WorkflowPlan::Stages& WorkflowPlan::getFinallyStages() const
{
    return _finalizationStages;
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

void WorkflowPlan::addNestedWorkflowStage(const QString& name, NestedWorkflowFunction function, JobThreadAffinity threadAffinity /*= JobThreadAffinity::CurrentWorkerThread*/, double weight /*= 1.0*/)
{
	Stage stage(name, ConcurrencyMode::Sequential, Jobs{ Job(name, NestedWorkflowJob{ std::move(function) }, threadAffinity, weight) });

	_stages.push_back(std::move(stage));
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

const WorkflowPlan::Jobs& WorkflowPlan::Stage::getJobs() const
{
	return _jobs;
}

WorkflowPlan::Jobs& WorkflowPlan::Stage::getJobs()
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
    for (auto& job : stage.getJobs()) {
        job.setWorkflowContext(_workflowContext);
    }

    _stages.emplace_back(std::move(stage));
}

void WorkflowPlan::addSequentialStage(QString name, Jobs jobs, double weight /*= 1.0*/)
{
    if (jobs.empty()) {
        qWarning() << "Attempted to add empty sequential stage:" << name;
	    return;
    }
        
    addStage(Stage(std::move(name), ConcurrencyMode::Sequential, std::move(jobs), weight));
}

void WorkflowPlan::addParallelStage(QString name, Jobs jobs, double weight /*= 1.0*/)
{
    if (jobs.empty()) {
        qWarning() << "Attempted to add empty parallel stage:" << name;
        return;
    }

    addStage(Stage(std::move(name), ConcurrencyMode::Parallel, std::move(jobs), weight));
}

void WorkflowPlan::addBatchedParallelStage(const QString& name, Jobs jobs, std::size_t batchSize)
{
	if (batchSize == 0)
		throw std::invalid_argument("batchSize must be greater than zero");

	if (jobs.empty())
		return;

	std::size_t batchIndex = 0;

	for (std::size_t begin = 0; begin < jobs.size(); begin += batchSize) {
		const auto end = std::min(begin + batchSize, jobs.size());

		Jobs batchJobs;
		batchJobs.reserve(end - begin);

		for (std::size_t i = begin; i < end; ++i)
			batchJobs.emplace_back(std::move(jobs[i]));

		addParallelStage(QString("%1 batch %2").arg(name).arg(++batchIndex), std::move(batchJobs));
	}
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

}
