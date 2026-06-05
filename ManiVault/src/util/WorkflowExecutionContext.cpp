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
	auto progressRoot   = WorkflowProgressNode::createRoot(Type::Workflow, name);
	auto state          = std::make_shared<WorkflowExecutionState>(reportRoot, progressRoot, std::move(executionOptions));
	auto context        = std::make_shared<WorkflowExecutionContext>(
        name,
        reportRoot,
        progressRoot,
        state,
        task
    );

    context->_type          = Type::Workflow;
    context->_id            = QUuid::createUuid();
    context->_executionPath = { name };

    return context;
}

SharedWorkflowExecutionContext WorkflowExecutionContext::createChild(Type type, const QString& name, double weight, WorkflowPlan::JobProgressMode progressMode)
{
    if (!_reportNode && !_progressNode && !_state)
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
        progressChild = WorkflowProgressNode::createRoot(type, name, effectiveWeight);
    }
    else {
        progressChild = _progressNode->createChild(type, name, effectiveWeight);
    }

    auto child = std::make_shared<WorkflowExecutionContext>(name, _reportNode->createChild(name), progressChild, _state, _task, progressMode);

    child->_type            = type;
    child->_parentId        = _id;
    child->_parent          = shared_from_this();
    child->_executionPath   = _executionPath;
    child->_executionPath.append(name);
    
    return child;
}

SharedWorkflowExecutionContext WorkflowExecutionContext::createWorkflowChild(const QString& name, double weight, WorkflowPlan::JobProgressMode progressMode)
{
    auto childReportNode    = _reportNode ? _reportNode->createChild(name) : nullptr;
    auto childProgressNode  = _progressNode ? _progressNode->createChild(Type::NestedWorkflow, name, weight) : nullptr;
    auto child              = std::make_shared<WorkflowExecutionContext>(name, childReportNode, childProgressNode, _state, _task, progressMode);

    child->_type            = Type::Workflow;
    child->_parentId        = _id;
    child->_parent          = shared_from_this();
    child->_executionPath   = _executionPath;

    child->_executionPath << name;

    return child;
}

SharedWorkflowExecutionContext WorkflowExecutionContext::createNestedWorkflowChild(const QString& name, double weight, WorkflowPlan::JobProgressMode progressMode)
{
    return createTypedChild(Type::NestedWorkflow, name, weight, progressMode);
}

SharedWorkflowExecutionContext WorkflowExecutionContext::createSequentialStageChild(const QString& name, double weight, WorkflowPlan::JobProgressMode progressMode)
{
    return createTypedChild(Type::SequentialStage, name, weight, progressMode);
}

SharedWorkflowExecutionContext WorkflowExecutionContext::createParallelStageChild(const QString& name, double weight, WorkflowPlan::JobProgressMode progressMode)
{
    return createTypedChild(Type::ParallelStage, name, weight, progressMode);
}

SharedWorkflowExecutionContext WorkflowExecutionContext::createJobChild(const QString& name, double weight, WorkflowPlan::JobProgressMode progressMode)
{
    return createTypedChild(Type::Job, name, weight, progressMode);
}

SharedWorkflowExecutionContext WorkflowExecutionContext::createTypedChild(Type type, const QString& name, double weight, WorkflowPlan::JobProgressMode progressMode)
{
    return createChild(type, name, weight, progressMode);
}

bool WorkflowExecutionContext::isRootExecution() const
{
    return _parent.expired();
}

void WorkflowExecutionContext::reportStarted() const
{
    info(_name, {}, makeLifecycleDetails("started"));

    if (_progressNode)
        _progressNode->markRunning();
}

void WorkflowExecutionContext::reportFinished(std::uint64_t durationMs) const
{
    info(_name, {}, makeLifecycleDetails("finished", durationMs));

    if (_progressNode) {
        _progressNode->markCompleted();
    }
}

void WorkflowExecutionContext::reportFailed(SeverityLevel severity, const QString& errorMessage, QVariantMap extraDetails /*= {}*/) const
{
    auto details = makeLifecycleDetails("failed");
    details["error"] = errorMessage;

    for (auto it = extraDetails.begin(); it != extraDetails.end(); ++it)
        details[it.key()] = it.value();

    message(severity, _name, {}, details);

    if (_progressNode)
        _progressNode->markFailed();
}

void WorkflowExecutionContext::reportSkipped(const QString& reason) const
{
    auto details = makeLifecycleDetails("skipped");
    details["reason"] = reason;

    warning(_name, {}, details);

    if (_progressNode)
        _progressNode->markSkipped();
}

void WorkflowExecutionContext::reportStageSummary(const WorkflowStageSummary& summary) const
{
    auto details = makeLifecycleDetails("summary", summary.durationMs);

    const auto summaryMap = summary.toVariantMap();

    for (auto it = summaryMap.begin(); it != summaryMap.end(); ++it)
        details[it.key()] = it.value();

    info(_name, {}, details);
}

QVariantMap WorkflowExecutionContext::makeLifecycleDetails(const QString& event, std::uint64_t durationMs /*= 0*/) const
{
    QVariantMap details;

    details["event"]    = event;
    details["entity"]   = getWorkflowExecutionNodeTypeName(_type);
    details["name"]     = _name;
    details["depth"]    = getDepth();
    details["path"]     = getExecutionPath(" / ");
    details["id"]       = _id.toString(QUuid::WithoutBraces);
    details["parentId"] = _parentId.toString(QUuid::WithoutBraces);
    details["threadId"] = QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()));

    if (durationMs > 0)
        details["durationMs"] = QVariant::fromValue<qulonglong>(durationMs);

    return details;
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
    static QMutex mutex;
    QMutexLocker lock(&mutex);

    const auto maxDepth = _state ? _state->getExecutionOptions()._maxConsoleLogDepth : std::numeric_limits<int>::max();

    const auto message = WorkflowConsoleFormatter::format(SeverityLevel::Info, text, location, details, maxDepth);

    if (!message.isEmpty())
        qDebug().noquote() << message;

    if (_reportNode) {
        _reportNode->addMessage(SeverityLevel::Info, getName(), text, location, details);
    }
}

void WorkflowExecutionContext::warning(QString text, QString location, QVariantMap details) const
{
    static QMutex mutex;
    QMutexLocker lock(&mutex);

    const auto maxDepth = _state ? _state->getExecutionOptions()._maxConsoleLogDepth : std::numeric_limits<int>::max();
		
    const auto message = WorkflowConsoleFormatter::format(SeverityLevel::Warning, text, location, details, maxDepth);

    if (!message.isEmpty())
        qDebug().noquote() << message;

	if (_reportNode)
		_reportNode->addMessage(SeverityLevel::Warning, getName(), std::move(text), std::move(location), std::move(details));
}

void WorkflowExecutionContext::error(QString text, QString location, QVariantMap details) const
{
    static QMutex mutex;
    QMutexLocker lock(&mutex);

    const auto maxDepth = _state ? _state->getExecutionOptions()._maxConsoleLogDepth : std::numeric_limits<int>::max();

    const auto message = WorkflowConsoleFormatter::format(SeverityLevel::Error, text, location, details, maxDepth);

    if (!message.isEmpty())
		qDebug().noquote() << message;

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


QString WorkflowExecutionContext::getExecutionPath(const QString& separator /*= "/"*/) const
{
    return _executionPath.join(separator);
}

std::int32_t WorkflowExecutionContext::getDepth() const
{
	return _executionPath.isEmpty() ? 0 : static_cast<std::int32_t>(_executionPath.size()) - 1;
}

QUuid WorkflowExecutionContext::getId() const
{
    return _id;
}

QUuid WorkflowExecutionContext::getParentId() const
{
    return _parentId;
}

void WorkflowExecutionContext::publishResult(const QVariantMap& values)
{
	for (auto it = values.constBegin(); it != values.constEnd(); ++it)
		publishResultValue(it.key(), it.value());
}

SharedWorkflowExecutionContext WorkflowExecutionContext::getResultScope()
{
    auto current = shared_from_this();

    while (current) {
        if (current->_type == Type::Workflow || current->_type == Type::NestedWorkflow) {
            return current;
        }

        current = current->_parent.lock();
    }

    return {};
}

}

