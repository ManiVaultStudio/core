// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowResultFuture.h"

namespace mv::util
{

void WorkflowResultFuture::State::setException(std::exception_ptr e)
{
	QMutexLocker locker(&mutex);
	exception = std::move(e);
}

std::exception_ptr WorkflowResultFuture::State::getException() const
{
	QMutexLocker locker(&mutex);
	return exception;
}

bool WorkflowResultFuture::State::hasException() const
{
	QMutexLocker locker(&mutex);
	return exception != nullptr;
}

void WorkflowResultFuture::State::rethrowExceptionIfAny() const
{
	if (auto exception = getException())
		std::rethrow_exception(exception);
}

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

SharedWorkflowResult WorkflowResultFuture::result() const
{
	if (!_state)
		return {};

	return _state->future.result();
}

const QFuture<SharedWorkflowResult>& WorkflowResultFuture::getFuture() const
{
	Q_ASSERT(_state);
	return _state->future;
}

Task* WorkflowResultFuture::getTask() const
{
	return _state ? _state->task.data() : nullptr;
}

QFutureWatcher<SharedWorkflowResult>* WorkflowResultFuture::getWatcher() const
{
	return _state ? _state->watcher.data() : nullptr;
}

WorkflowResultFuture WorkflowResultFuture::makeReady(const SharedWorkflowResult& result)
{
	auto state = std::make_shared<State>();

	QPromise<SharedWorkflowResult> promise;

	state->future = promise.future();
	promise.start();
	promise.addResult(result);
	promise.finish();

	return WorkflowResultFuture(state);
}

WorkflowResultFuture WorkflowResultFuture::fromFuture(QFuture<SharedWorkflowResult> future)
{
	auto state    = std::make_shared<State>();
	state->future = std::move(future);
	return WorkflowResultFuture(state);
}
}
