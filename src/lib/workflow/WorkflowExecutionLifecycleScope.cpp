// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowExecutionLifecycleScope.h"

using namespace mv::util;

namespace mv::workflow
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

void WorkflowExecutionLifecycleScope::fail(SeverityLevel severity, const QString& message, QVariantMap details /*= {}*/)
{
    if (_context && !_finished) {
        _context->reportFailed(severity, message, std::move(details));

        _finished = true;
    }
}

void WorkflowExecutionLifecycleScope::failWithoutMessage()
{
    if (_context && !_finished) {
        _context->markFailed();
        _finished = true;
    }
}

void WorkflowExecutionLifecycleScope::finish(std::uint64_t durationMs)
{
    if (_context && !_finished) {
        _context->reportFinished(durationMs);
        _finished = true;
    }
}

void WorkflowExecutionLifecycleScope::finish()
{
	finish(elapsedMS());
}

std::uint64_t WorkflowExecutionLifecycleScope::elapsedMS() const
{
	return static_cast<std::uint64_t>(_timer.elapsed());
}

}
