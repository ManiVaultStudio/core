// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Workflow.h"

#ifdef _DEBUG
	#define WORKFLOW_VERBOSE
#endif

namespace mv::util
{

Workflow::Workflow(WorkflowPlan workflowPlan, SharedOperationContext operationContext /*= {}*/) :
	//_title(std::move(title)),
	_duration(0),
    _workflowPlan(std::move(workflowPlan)),
    _operationContext(operationContext ? std::move(operationContext) : std::make_shared<OperationContext>())
{
#ifdef WORKFLOW_VERBOSE
    printLine(_title, "Create");
#endif
}

QString Workflow::getTitle() const
{
	return _workflowPlan.getTitle();
}

void Workflow::start()
{
#ifdef WORKFLOW_VERBOSE
    printLine(_title, "Start", 1);
#endif

    _duration = 0;

    
}

void Workflow::cancel()
{
#ifdef WORKFLOW_VERBOSE
    printLine(_title, "Cancel", 1);
#endif

	//_runner.cancel();
}

quint64 Workflow::getDuration() const
{
	return _duration;
}

void Workflow::setDoneCallback(DoneCallback callback)
{
    _doneCallback = std::move(callback);
}

void Workflow::handleDone(QtTaskTree::DoneWith doneWith)
{
    //_duration = static_cast<quint64>(_timer.elapsed());

    const bool success = (doneWith == QtTaskTree::DoneWith::Success);

    //_task.setFinished();

    //if (_doneCallback)
    //    _doneCallback(success, success ? QString{} : _result->_errorMessage);
}

//void Workflow::setupTree(QtTaskTree::QTaskTree& context)
//{
//    Q_UNUSED(context)
//}
//
//WorkflowRuntimeContextStorage& Workflow::contextStorage()
//{
//	return _contextStorage;
//}
//
//const WorkflowRuntimeContextStorage& Workflow::contextStorage() const
//{
//	return _contextStorage;
//}

const std::shared_ptr<OperationContext>& Workflow::getConstOperationContext() const
{
	return _operationContext;
}

SharedOperationContext Workflow::getOperationContext()
{
	return _operationContext;
}

//void Workflow::registerStorageHandlers(QtTaskTree::QTaskTree& tree)
//{
//#ifdef WORKFLOW_VERBOSE
//    printLine(_title, "Register storage handlers", 1);
//#endif
//
//    //tree.onStorageSetup(
//    //    _contextStorage,
//    //    [this](WorkflowRuntimeContext& context) {
//	   //     context._workflowContext = std::move(_initialWorkflowContext);
//	   //     context._errorMessage.clear();
//	   //     context._success = false;
//
//	   //     setupStorage(context);
//    //});
//
//    //tree.onStorageDone(
//    //    _contextStorage,
//    //    [this](const WorkflowRuntimeContext& context) {
//    //        onStorageDone(context);
//    //});
//}

}
