// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AbstractWorkflow.h"

namespace mv::util
{

AbstractWorkflow::AbstractWorkflow(UniqueWorkflowContext workflowContext, QString title, QObject* parent) :
	QObject(parent),
	_title(std::move(title)),
	_duration(0),
    _initialWorkflowContext(std::move(workflowContext))
{
}

const QString& AbstractWorkflow::title() const
{
	return _title;
}

void AbstractWorkflow::start()
{
    _duration = 0;

	_timer.restart();

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
	_runner.cancel();
}

quint64 AbstractWorkflow::getDuration() const
{
	return _duration;
}

void AbstractWorkflow::handleDone(QtTaskTree::DoneWith doneWith)
{
    _duration = static_cast<quint64>(_timer.elapsed());

    const bool success = (doneWith == QtTaskTree::DoneWith::Success);

	emit finished(success, QString{});
}

void AbstractWorkflow::setupTree(QtTaskTree::QTaskTree& context)
{
}

WorkflowRuntimeContextStorage& AbstractWorkflow::contextStorage()
{
	return _contextStorage;
}

const WorkflowRuntimeContextStorage& AbstractWorkflow::contextStorage() const
{
	return _contextStorage;
}

void AbstractWorkflow::registerStorageHandlers(QtTaskTree::QTaskTree& tree)
{
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
