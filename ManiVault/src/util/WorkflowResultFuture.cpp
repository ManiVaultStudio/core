// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowResultFuture.h"

namespace mv::util
{
WorkflowResultFuture::WorkflowResultFuture() = default;

WorkflowResultFuture::WorkflowResultFuture(std::shared_ptr<State> state): _state(std::move(state))
{
}

bool WorkflowResultFuture::isValid() const
{
	return _state && _state->future.isValid();
}

bool WorkflowResultFuture::isFinished() const
{
	return _state && _state->future.isFinished();
}

void WorkflowResultFuture::waitForFinished() const
{
	if (_state)
		_state->future.waitForFinished();
}

WorkflowResult WorkflowResultFuture::result() const
{
	if (!_state)
		return {};

	return _state->future.result();
}

const QFuture<WorkflowResult>& WorkflowResultFuture::getFuture() const
{
	Q_ASSERT(_state);
	return _state->future;
}

Task* WorkflowResultFuture::getTask() const
{
	return _state ? _state->task.data() : nullptr;
}

QFutureWatcher<WorkflowResult>* WorkflowResultFuture::getWatcher() const
{
	return _state ? _state->watcher.data() : nullptr;
}

WorkflowResultFuture WorkflowResultFuture::makeReady(const WorkflowResult& result)
{
	auto state = std::make_shared<State>();

	QPromise<WorkflowResult> promise;
	state->future = promise.future();
	promise.start();
	promise.addResult(result);
	promise.finish();

	return WorkflowResultFuture(state);
}

WorkflowResultFuture WorkflowResultFuture::fromFuture(QFuture<WorkflowResult> future)
{
	auto state    = std::make_shared<State>();
	state->future = std::move(future);
	return WorkflowResultFuture(state);
}
}
