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

WorkflowExecutionContext::WorkflowExecutionContext(QString name, ReportNodePtr reportNode, ProgressNodePtr progressNode, StatePtr state) :
	_name(std::move(name)),
	_reportNode(std::move(reportNode)),
	_progressNode(std::move(progressNode)),
	_state(std::move(state))
{
}

WorkflowExecutionContext WorkflowExecutionContext::makeRoot(const QString& name, Task* task /*= nullptr*/)
{
	auto reportRoot   = std::make_shared<WorkflowReportNode>(name);
	auto progressRoot = std::make_shared<WorkflowProgressNode>(1.0);
	auto state        = std::make_shared<WorkflowExecutionState>(reportRoot, progressRoot);

    return {
        name,
        reportRoot,
        progressRoot,
        state
    };
}

WorkflowExecutionContext WorkflowExecutionContext::createChild(const QString& name, double weight) const
{
	if (!_reportNode || !_progressNode || !_state)
		return {};

	return {
		name,
		_reportNode->createChild(name),
		_progressNode->createChild(weight),
		_state
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

	if (_state)
		_state->notifyMessagesChanged();

	//if (_task)
	//    _task->setStatusText(text);
}

void WorkflowExecutionContext::warning(const QString& text, const QString& source) const
{
	if (_reportNode)
		_reportNode->addMessage(WorkflowMessageLevel::Warning, source, text);

	if (_state)
		_state->notifyMessagesChanged();

	//if (_task)
	//    _task->addWarning(text);
}

void WorkflowExecutionContext::error(const QString& text, const QString& source) const
{
	if (_reportNode)
		_reportNode->addMessage(WorkflowMessageLevel::Error, source, text);

	if (_state)
		_state->notifyMessagesChanged();

	//if (_task)
	//    _task->addError(text);
}

void WorkflowExecutionContext::setProgress(double value) const
{
	if (_progressNode)
		_progressNode->setProgress(value);

	const double overall = _state ? _state->getOverallProgress() : 0.0;

	qDebug() << "Setting local progress of context" << _name << "to" << value
		<< ", overall progress =" << overall;

	if (_state)
		_state->notifyProgressChanged();

	//if (_task && _state)
	//	_task->setProgress(overall);
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

WorkflowExecutionContext* WorkflowExecutionContext::current()
{
	return currentWorkflowExecutionContext;
}

const WorkflowExecutionContext* WorkflowExecutionContext::currentConst()
{
	return currentWorkflowExecutionContext;
}

void WorkflowExecutionContext::setCurrent(WorkflowExecutionContext* context)
{
    currentWorkflowExecutionContext = context;
}

}
