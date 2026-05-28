// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowExecutionLifecycleScope.h"

namespace mv::util
{

WorkflowExecutionLifecycleScope::WorkflowExecutionLifecycleScope(SharedWorkflowExecutionContext context) :
	_context(std::move(context))
{
	if (_context) {
		_timer.start();
		_context->reportStarted();
	}
}

WorkflowExecutionLifecycleScope::~WorkflowExecutionLifecycleScope()
{
	if (_context && !_finished) {
        finish();
	}
}

void WorkflowExecutionLifecycleScope::fail(const QString& message)
{
	if (_context && !_finished) {
		_context->reportFailed(message);
		_finished = true;
	}
}

void WorkflowExecutionLifecycleScope::finish()
{
	if (_context && !_finished) {
		_context->reportFinished(
			static_cast<std::uint64_t>(_timer.elapsed())
		);

		_finished = true;
	}
}

std::uint64_t WorkflowExecutionLifecycleScope::elapsedMs() const
{
	return static_cast<std::uint64_t>(_timer.elapsed());
}
}
