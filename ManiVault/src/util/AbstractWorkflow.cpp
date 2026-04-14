// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AbstractWorkflow.h"

#ifdef _DEBUG
	#define ABSTRACT_WORKFLOW_VERBOSE
#endif

namespace mv::util
{

AbstractWorkflow::AbstractWorkflow(UniqueWorkflowContext workflowContext, QString title, SharedOperationContext operationContext /*= {}*/) :
	_title(std::move(title)),
	_duration(0),
    _initialWorkflowContext(std::move(workflowContext)),
    _operationContext(operationContext ? std::move(operationContext) : std::make_shared<OperationContext>())
{
#ifdef ABSTRACT_WORKFLOW_VERBOSE
    printLine(_title, "Create");
#endif
}

const QString& AbstractWorkflow::title() const
{
	return _title;
}

void AbstractWorkflow::start()
{
#ifdef ABSTRACT_WORKFLOW_VERBOSE
    printLine(_title, "Start", 1);
#endif

    _duration = 0;

    //_task.setRunning();

    initResult(_result);

    _runner.start(
        makeRecipe(),
        [this](QtTaskTree::QTaskTree& tree) {
            registerStorageHandlers(tree);
        },
        [this](QtTaskTree::DoneWith doneWith) {
            handleDone(doneWith);
    });
}

void AbstractWorkflow::cancel()
{
#ifdef ABSTRACT_WORKFLOW_VERBOSE
    printLine(_title, "Cancel", 1);
#endif

	_runner.cancel();
}

quint64 AbstractWorkflow::getDuration() const
{
	return _duration;
}

void AbstractWorkflow::setDoneCallback(DoneCallback callback)
{
    _doneCallback = std::move(callback);
}

void AbstractWorkflow::handleDone(QtTaskTree::DoneWith doneWith)
{
    //_duration = static_cast<quint64>(_timer.elapsed());

    const bool success = (doneWith == QtTaskTree::DoneWith::Success);

    //_task.setFinished();

    if (_doneCallback)
        _doneCallback(success, success ? QString{} : _result->_errorMessage);
}

void AbstractWorkflow::setupTree(QtTaskTree::QTaskTree& context)
{
    Q_UNUSED(context)
}

WorkflowRuntimeContextStorage& AbstractWorkflow::contextStorage()
{
	return _contextStorage;
}

const WorkflowRuntimeContextStorage& AbstractWorkflow::contextStorage() const
{
	return _contextStorage;
}

const std::shared_ptr<OperationContext>& AbstractWorkflow::getConstOperationContext() const
{
	return _operationContext;
}

SharedOperationContext AbstractWorkflow::getOperationContext()
{
	return _operationContext;
}

void AbstractWorkflow::registerStorageHandlers(QtTaskTree::QTaskTree& tree)
{
#ifdef ABSTRACT_WORKFLOW_VERBOSE
    printLine(_title, "Register storage handlers", 1);
#endif

    tree.onStorageSetup(
        _contextStorage,
        [this](WorkflowRuntimeContext& context) {
	        context._workflowContext = std::move(_initialWorkflowContext);
	        context._errorMessage.clear();
	        context._success = false;

	        setupStorage(context);
    });

    tree.onStorageDone(
        _contextStorage,
        [this](const WorkflowRuntimeContext& context) {
            onStorageDone(context);
    });
}

}
