// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowExecutionContext.h"

namespace mv::util
{

namespace
{
    thread_local WorkflowExecutionContext* currentWorkflowExecutionContext = nullptr;
}

WorkflowExecutionContext::WorkflowExecutionContext() = default;

WorkflowExecutionContext::WorkflowExecutionContext(QString name, ReportNodePtr reportNode, ProgressNodePtr progressNode, StatePtr state, SharedThreadPool threadPool, Task* task /*= nullptr*/, WorkflowPlan::JobProgressMode progressMode /*= WorkflowPlan::JobProgressMode::Automatic*/) :
	_name(std::move(name)),
	_reportNode(std::move(reportNode)),
	_progressNode(std::move(progressNode)),
	_state(std::move(state)),
    _threadPool(std::move(threadPool)),
    _task(task),
    _progressMode(progressMode)
{
}

WorkflowExecutionContext WorkflowExecutionContext::makeRoot(const QString& name, Task* task /*= nullptr*/, WorkflowExecutionOptions executionOptions /*= {}*/)
{
	auto reportRoot   = std::make_shared<WorkflowReportNode>(name);
	auto progressRoot = std::make_shared<WorkflowProgressNode>(1.0);
	auto state        = std::make_shared<WorkflowExecutionState>(reportRoot, progressRoot, executionOptions);

    auto threadPool = std::make_shared<QThreadPool>();

	threadPool->setObjectName("WorkflowExecutorPool");
    threadPool->setMaxThreadCount(state->getExecutionOptions()._parallel ? state->getExecutionOptions()._maxWorkerThreadCount : 1);
    threadPool->setExpiryTimeout(30'000);

	return {
        name,
        reportRoot,
        progressRoot,
        state,
        threadPool,
        task
    };
}

WorkflowExecutionContext WorkflowExecutionContext::createChild(const QString& name, double weight, WorkflowPlan::JobProgressMode progressMode) const
{
	if (!_reportNode || !_progressNode || !_state)
		return {};

    if (getProgress() > 0.0) {
        qWarning() << "Adding child to progress node after progress already started"
            << "current progress =" << getProgress()
            << "child weight =" << weight;
    }

    WorkflowProgressNode::Ptr progressChild;

    if (_progressMode == WorkflowPlan::JobProgressMode::Atomic)
        progressChild = std::make_shared<WorkflowProgressNode>(weight);
    else
        progressChild = _progressNode->createChild(weight);

	return {
		name,
		_reportNode->createChild(name),
        progressChild,
		_state,
        _threadPool,
        _task,
        progressMode
    };
}

bool WorkflowExecutionContext::hasProgressChildren() const
{
	return _progressNode ? _progressNode->hasChildren() : false;
}

bool WorkflowExecutionContext::isValid() const
{
	return static_cast<bool>(_reportNode) && static_cast<bool>(_progressNode) && static_cast<bool>(_state);
}

QString WorkflowExecutionContext::getName() const
{
	return _name;
}

void WorkflowExecutionContext::info(const QString& text, const QString& source) const
{
	if (_reportNode)
		_reportNode->addMessage(WorkflowMessageLevel::Info, source, text);

	//if (_task)
	//    _task->setStatusText(text);
}

void WorkflowExecutionContext::warning(const QString& text, const QString& source) const
{
	if (_reportNode)
		_reportNode->addMessage(WorkflowMessageLevel::Warning, source, text);

	//if (_task)
	//    _task->addWarning(text);
}

void WorkflowExecutionContext::error(const QString& text, const QString& source) const
{
	if (_reportNode)
		_reportNode->addMessage(WorkflowMessageLevel::Error, source, text);

	//if (_task)
	//    _task->addError(text);
}

void WorkflowExecutionContext::setProgress(double value) const
{
	if (_progressNode)
		_progressNode->setProgress(value);

	const double overall = _state ? _state->getOverallProgress() : 0.0;

	//qDebug() << "Setting local progress of context" << _name << "to" << value
	//	<< ", overall progress =" << overall;

	if (_task && _state)
		_task->setProgress(static_cast<float>(overall));
}

double WorkflowExecutionContext::getProgress() const
{
	return _progressNode ? _progressNode->getProgress() : 0.0;
}

WorkflowExecutionContext::ReportNodePtr WorkflowExecutionContext::getReportNode() const
{
	return _reportNode;
}

WorkflowExecutionContext::ProgressNodePtr WorkflowExecutionContext::getProgressNode() const
{
	return _progressNode;
}

WorkflowExecutionContext::StatePtr WorkflowExecutionContext::getState() const
{
	return _state;
}

QThreadPool& WorkflowExecutionContext::getThreadPool()
{
    Q_ASSERT(_threadPool);

    return *_threadPool;
}

WorkflowExecutionContext* WorkflowExecutionContext::current()
{
	return currentWorkflowExecutionContext;
}

const WorkflowExecutionContext* WorkflowExecutionContext::currentConst()
{
	return currentWorkflowExecutionContext;
}

WorkflowPlan::JobProgressMode WorkflowExecutionContext::getProgressMode() const
{
	return _progressMode;
}

void WorkflowExecutionContext::setCurrent(WorkflowExecutionContext* context)
{
    currentWorkflowExecutionContext = context;
}

}
