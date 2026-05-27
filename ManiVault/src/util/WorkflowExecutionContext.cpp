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

QString WorkflowExecutionContext::getWorkflowExecutionContextTypeName(Type type)
{
    switch (type) {
	    case Type::Workflow:
	        return "workflow";

	    case Type::NestedWorkflow:
	        return "nested-workflow";

	    case Type::SequentialStage:
	        return "sequential-stage";

	    case Type::ParallelStage:
	        return "parallel-stage";

	    case Type::Job:
	        return "job";
    }

    return "unknown";
}

SharedWorkflowExecutionContext WorkflowExecutionContext::makeRoot(const QString& name, Task* task /*= nullptr*/, WorkflowExecutionOptions executionOptions /*= {}*/)
{
	auto reportRoot     = std::make_shared<WorkflowReportNode>(name);
	auto progressRoot   = std::make_shared<WorkflowProgressNode>(1.0);
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
    context->_resultScope   = context->_id.toString(QUuid::WithoutBraces);

    return context;
}

SharedWorkflowExecutionContext WorkflowExecutionContext::createChild(const QString& name, double weight, WorkflowPlan::JobProgressMode progressMode) const
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
    child->_resultScope = _resultScope;

    return child;
}

SharedWorkflowExecutionContext WorkflowExecutionContext::createWorkflowChild(const QString& name, double weight, WorkflowPlan::JobProgressMode progressMode) const
{
    auto childReportNode    = _reportNode ? _reportNode->createChild(name) : nullptr;
    auto childProgressNode  = _progressNode ? _progressNode->createChild(weight) : nullptr;
    auto child              = std::make_shared<WorkflowExecutionContext>(
        name,
        childReportNode,
        childProgressNode,
        _state,
        _task,
        progressMode
    );

    child->_type = Type::Workflow;
    child->_parentId = _id;
    child->_executionPath = _executionPath;
    child->_executionPath << name;
    child->_resultScope = makeChildResultScope(child->_id);

    return child;
}

SharedWorkflowExecutionContext WorkflowExecutionContext::createNestedWorkflowChild(const QString& name, double weight, WorkflowPlan::JobProgressMode progressMode) const
{
    auto child = createTypedChild(name, Type::NestedWorkflow, weight, progressMode);

    child->_resultScope = makeChildResultScope(child->_id);

    return child;
}

SharedWorkflowExecutionContext WorkflowExecutionContext::createSequentialStageChild(const QString& name, double weight, WorkflowPlan::JobProgressMode progressMode) const
{
    return createTypedChild(name, Type::SequentialStage, weight, progressMode);
}

SharedWorkflowExecutionContext WorkflowExecutionContext::createParallelStageChild(const QString& name, double weight, WorkflowPlan::JobProgressMode progressMode) const
{
    return createTypedChild(name, Type::ParallelStage, weight, progressMode);
}

SharedWorkflowExecutionContext WorkflowExecutionContext::createJobChild(const QString& name, double weight, WorkflowPlan::JobProgressMode progressMode) const
{
    return createTypedChild(name, Type::Job, weight, progressMode);
}

SharedWorkflowExecutionContext WorkflowExecutionContext::createTypedChild(const QString& name, Type type, double weight, WorkflowPlan::JobProgressMode progressMode) const
{
    auto child = createChild(name, weight, progressMode);
    child->_type = type;

    return child;
}

void WorkflowExecutionContext::reportStarted() const
{
    info(_name, {}, makeLifecycleDetails("started"));
}

void WorkflowExecutionContext::reportFinished(std::uint64_t durationMs) const
{
    info(_name, {}, makeLifecycleDetails("finished", durationMs));
}

void WorkflowExecutionContext::reportFailed(const QString& errorMessage) const
{
    auto details = makeLifecycleDetails("failed");
    details["error"] = errorMessage;

    error(_name, {}, details);
}

void WorkflowExecutionContext::reportSkipped(const QString& reason) const
{
    auto details = makeLifecycleDetails("skipped");
    details["reason"] = reason;

    warning(_name, {}, details);
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
    details["entity"]   = getWorkflowExecutionContextTypeName(_type);
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

    qDebug().noquote() << WorkflowConsoleFormatter::format(SeverityLevel::Info, text, location, details);

    if (_reportNode) {
        _reportNode->addMessage(SeverityLevel::Info, getName(), text, location, details);
    }
}

void WorkflowExecutionContext::warning(QString text, QString location, QVariantMap details) const
{
    static QMutex mutex;
    QMutexLocker lock(&mutex);

    qDebug().noquote() << WorkflowConsoleFormatter::format(SeverityLevel::Warning, text, location, details);

	if (_reportNode)
		_reportNode->addMessage(SeverityLevel::Warning, getName(), std::move(text), std::move(location), std::move(details));
}

void WorkflowExecutionContext::error(QString text, QString location, QVariantMap details) const
{
    static QMutex mutex;
    QMutexLocker lock(&mutex);

    qDebug().noquote() << WorkflowConsoleFormatter::format(SeverityLevel::Error, text, location, details);

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

QVariant WorkflowExecutionContext::getResultValue(const QString& localKey) const
{
	return _state->getResultValue(scopedResultKey(localKey));
}

QVariant WorkflowExecutionContext::takeResultValue(const QString& localKey) const
{
	return _state->takeResultValue(scopedResultKey(localKey));
}

bool WorkflowExecutionContext::hasResultValue(const QString& localKey) const
{
	return _state->hasResultValue(scopedResultKey(localKey));
}

QVariantMap WorkflowExecutionContext::getResultValues() const
{
    return _state->getResultValues(_resultScope);
}

QVariantMap WorkflowExecutionContext::takeResultValues() const
{
    return _state->takeResultValues(_resultScope);
}

QString WorkflowExecutionContext::getResultScope() const
{
	return _resultScope;
}

QString WorkflowExecutionContext::makeChildResultScope(const QUuid& childId) const
{
    const auto childIdString = childId.toString(QUuid::WithoutBraces);

    return _resultScope.isEmpty() ? childIdString : QString("%1/%2").arg(_resultScope, childIdString);
}

QString WorkflowExecutionContext::scopedResultKey(const QString& localKey) const
{
	if (localKey.trimmed().isEmpty())
		throw std::invalid_argument("Workflow result key may not be empty");

	return QString("%1/%2").arg(_resultScope, localKey);
}
}
