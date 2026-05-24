// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowExecutionContext.h"

namespace mv::util
{

WorkflowExecutionContext::WorkflowExecutionContext() :
    _id(QUuid::createUuid())
{
}

WorkflowExecutionContext::WorkflowExecutionContext(QString name, ReportNodePtr reportNode, ProgressNodePtr progressNode, StatePtr state, Task* task /*= nullptr*/, WorkflowPlan::JobProgressMode progressMode /*= WorkflowPlan::JobProgressMode::Automatic*/) :
	_name(std::move(name)),
    _id(QUuid::createUuid()),
    _executionPath({ _name }),
	_reportNode(std::move(reportNode)),
	_progressNode(std::move(progressNode)),
	_state(std::move(state)),
    _task(task),
    _progressMode(progressMode)
{
}

SharedWorkflowExecutionContext WorkflowExecutionContext::makeRoot(const QString& name, Task* task /*= nullptr*/, WorkflowExecutionOptions executionOptions /*= {}*/)
{
	auto reportRoot     = std::make_shared<WorkflowReportNode>(name);
	auto progressRoot   = std::make_shared<WorkflowProgressNode>(1.0);
	auto state          = std::make_shared<WorkflowExecutionState>(reportRoot, progressRoot, std::move(executionOptions));
    auto threadPool     = std::make_shared<QThreadPool>();

	threadPool->setObjectName("WorkflowExecutorPool");
    threadPool->setMaxThreadCount(64);//state->getExecutionOptions()._parallel ? state->getExecutionOptions()._maxWorkerThreadCount : 1);
    threadPool->setExpiryTimeout(30'000);

	return std::make_shared<WorkflowExecutionContext>(
        name,
        reportRoot,
        progressRoot,
        state,
        task
    );
}

SharedWorkflowExecutionContext WorkflowExecutionContext::createChild(const QString& name, double weight, WorkflowPlan::JobProgressMode progressMode)
{
    if (!_reportNode || !_progressNode || !_state)
        return {};

    const auto effectiveWeight = std::max(1.0, weight);

    if (getProgress() > 0.0 && effectiveWeight > 0.0) {
        qWarning() << "Adding child to progress node after progress already started"
            << "current progress =" << getProgress()
            << "child weight =" << effectiveWeight
            << "original weight =" << weight;
    }

    WorkflowProgressNode::Ptr progressChild;

    if (_progressMode == WorkflowPlan::JobProgressMode::Atomic) {
        progressChild = std::make_shared<WorkflowProgressNode>(effectiveWeight);
    }
    else {
        progressChild = _progressNode->createChild(effectiveWeight);
    }

    auto child = std::make_shared<WorkflowExecutionContext>(
        name,
        _reportNode->createChild(name),
        progressChild,
        _state,
        _task,
        progressMode
    );

    child->_parentId = _id;
    child->_executionPath = _executionPath;
    child->_executionPath.append(name);
    
    {
        std::lock_guard lock(_childrenMutex);
        _children.push_back(child);
    }

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

void WorkflowExecutionContext::message(SeverityLevel severity, QString text, QString location, QVariantMap details) const
{
	switch (severity) {
		case SeverityLevel::Info:
			info(std::move(text), std::move(location), std::move(details));
			break;

		case SeverityLevel::Warning:
			warning(std::move(text), std::move(location), std::move(details));
			break;

		case SeverityLevel::Error:
		case SeverityLevel::Fatal:
			error(std::move(text), std::move(location), std::move(details));
			break;
	}
}

void WorkflowExecutionContext::info(QString text, QString location, QVariantMap details) const
{
    qDebug() << "Info:" << text << ", location:" << location;

	if (_reportNode)
		_reportNode->addMessage(SeverityLevel::Info, getName(), std::move(text), std::move(location), std::move(details));
}

void WorkflowExecutionContext::warning(QString text, QString location, QVariantMap details) const
{
    qDebug() << "Warning:" << text << ", location:" << location;

	if (_reportNode)
		_reportNode->addMessage(SeverityLevel::Warning, getName(), std::move(text), std::move(location), std::move(details));
}

void WorkflowExecutionContext::error(QString text, QString location, QVariantMap details) const
{
    qDebug() << "Error:" << text << ", location:" << location;

	if (_reportNode)
		_reportNode->addMessage(SeverityLevel::Error, getName(), std::move(text), std::move(location), std::move(details));
}

void WorkflowExecutionContext::setProgress(double value) const
{
	if (_progressNode)
		_progressNode->setProgress(value);

	const double overall = _state ? _state->getOverallProgress() : 0.0;

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

WorkflowPlan::JobProgressMode WorkflowExecutionContext::getProgressMode() const
{
	return _progressMode;
}

void WorkflowExecutionContext::addPendingAsyncWork(WorkflowResultFuture future, const QString& label /*= {}*/)
{
    std::lock_guard lock(_pendingAsyncWorkMutex);

    _pendingAsyncWork.push_back({
        ._future = std::move(future),
        ._label = label
	});
}

void WorkflowExecutionContext::waitForPendingAsyncWork()
{
    while (true) {
        std::vector<PendingAsyncWork> pendingWork;

        {
            std::lock_guard lock(_pendingAsyncWorkMutex);

            if (_pendingAsyncWork.empty())
                break;

            pendingWork = std::move(_pendingAsyncWork);
            _pendingAsyncWork.clear();
        }

        for (auto& work : pendingWork) {
            try {
                work._future.waitForFinished();

                if (auto state = work._future.getState())
                    state->rethrowExceptionIfAny();

                const auto result = work._future.result();

                if (result && result->hasErrors()) {
                    throw ManiVaultException(
                        SeverityLevel::Error,
                        QString("Nested workflow failed: %1").arg(work._label),
                        "workflow.nested_failed",
                        getExecutionPath()
                    );
                }
            }
            catch (...) {
                throw;
            }
        }
    }
}

QString WorkflowExecutionContext::getExecutionPath(const QString& separator /*= "/"*/) const
{
    return _executionPath.join(separator);
}

QUuid WorkflowExecutionContext::getId() const
{
    return _id;
}

QUuid WorkflowExecutionContext::getParentId() const
{
    return _parentId;
}

}
