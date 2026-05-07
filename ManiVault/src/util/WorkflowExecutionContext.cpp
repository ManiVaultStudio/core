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

WorkflowExecutionContext::WorkflowExecutionContext() :
    _id(QUuid::createUuid())
{
}

WorkflowExecutionContext::WorkflowExecutionContext(QString name, ReportNodePtr reportNode, ProgressNodePtr progressNode, StatePtr state, SharedThreadPool threadPool, Task* task /*= nullptr*/, WorkflowPlan::JobProgressMode progressMode /*= WorkflowPlan::JobProgressMode::Automatic*/) :
	_name(std::move(name)),
    _id(QUuid::createUuid()),
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

    WorkflowExecutionContext child{
        name,
        _reportNode->createChild(name),
        progressChild,
        _state,
        _threadPool,
        _task,
        progressMode
    };

    child._parentId = _id;

    return child;
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

void WorkflowExecutionContext::info(const QString& text, const QString& source, const QString& scope, QVariantMap details) const
{
	if (_reportNode)
		_reportNode->addMessage(SeverityLevel::Info, source, text, scope, details);

	//if (_task)
	//    _task->setStatusText(text);
}

void WorkflowExecutionContext::warning(const QString& text, const QString& source, const QString& scope, QVariantMap details) const
{
	if (_reportNode)
		_reportNode->addMessage(SeverityLevel::Warning, source, text, scope, details);

	//if (_task)
	//    _task->addWarning(text);
}

void WorkflowExecutionContext::error(const QString& text, const QString& source, const QString& scope, QVariantMap details) const
{
	if (_reportNode)
		_reportNode->addMessage(SeverityLevel::Error, source, text, scope, details);

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

void WorkflowExecutionContext::addPendingAsyncWork(WorkflowResultFuture future, const QString& label /*= {}*/)
{
    _pendingAsyncWork.push_back({
        ._future = std::move(future),
        ._label = std::move(label)
    });
}

void WorkflowExecutionContext::waitForPendingAsyncWork()
{
    for (auto& pendingWork : _pendingAsyncWork) {
        if (auto state = getState()) {
            state->trace({
	            ._type = WorkflowTraceEventType::PendingAsyncWorkItemStarted,
	            ._name = pendingWork._label,
	            ._contextId = getId(),
	            ._parentContextId = getParentId(),
                ._threadId = QThread::currentThreadId(),
				._timestampNs = AbstractWorkflowTraceSink::currentTimestampNs()
            });
        }

        try {
            pendingWork._future.waitForFinished();
            pendingWork._future.getState()->rethrowExceptionIfAny();
        }
        catch (...) {
            if (auto state = getState()) {
                state->trace({
                    ._type = WorkflowTraceEventType::PendingAsyncWorkItemFailed,
                    ._name = pendingWork._label,
                    ._contextId = getId(),
                    ._parentContextId = getParentId(),
	                ._threadId = QThread::currentThreadId(),
	                ._timestampNs = AbstractWorkflowTraceSink::currentTimestampNs()
                });
            }

            throw;
        }

        if (auto state = getState()) {
            state->trace({
	            ._type = WorkflowTraceEventType::PendingAsyncWorkItemFinished,
	            ._name = pendingWork._label,
	            ._contextId = getId(),
	            ._parentContextId = getParentId(),
                ._threadId = QThread::currentThreadId(),
                ._timestampNs = AbstractWorkflowTraceSink::currentTimestampNs()
            });
        }
    }

    _pendingAsyncWork.clear();
}

QUuid WorkflowExecutionContext::getId() const
{
    return _id;
}

QUuid WorkflowExecutionContext::getParentId() const
{
    return _parentId;
}

std::size_t WorkflowExecutionContext::getPendingAsyncWorkCount() const
{
    return _pendingAsyncWork.size();
}

}
