// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowResultFuture.h"
#include "Task.h"

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

const std::future<SharedWorkflowResult>& WorkflowResultFuture::getFuture() const
{
	Q_ASSERT(_state);
	return _state->future;
}

Task* WorkflowResultFuture::getTask() const
{
	return _state ? _state->task.data() : nullptr;
}

}
