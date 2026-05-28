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
    if (auto exception = getException()) {
        try {
            std::rethrow_exception(exception);
        }
        catch (const std::exception& e) {
            qDebug() << "Workflow exception:" << e.what();
            throw;
        }
        catch (...) {
            qDebug() << "Unknown workflow exception";
            throw;
        }
    }
}

WorkflowResultFuture::WorkflowResultFuture() = default;

WorkflowResultFuture::WorkflowResultFuture(std::shared_ptr<State> state): _state(std::move(state))
{
}

SharedWorkflowResult WorkflowResultFuture::get() const
{
	Q_ASSERT(_state);

	auto result = _state->future.get();

	_state->rethrowExceptionIfAny();

	return result;
}

Task* WorkflowResultFuture::getTask() const
{
	return _state ? _state->task.data() : nullptr;
}

void WorkflowResultFuture::onFinished(QObject* receiver, std::function<void(SharedWorkflowResult)> callback)
{
	auto state = _state;

	std::thread([state, receiver, callback = std::move(callback)]() mutable {
		SharedWorkflowResult result;

		try {
			result = state->future.get();
		}
		catch (...) {
			state->setException(std::current_exception());
		}

		QMetaObject::invokeMethod(
			receiver,
			[state, callback = std::move(callback), result]() mutable {
				state->rethrowExceptionIfAny();
				callback(result);
			},
			Qt::QueuedConnection);
	}).detach();
}
}
