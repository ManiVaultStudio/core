// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowReporter.h"

namespace mv::util
{

bool WorkflowReporter::hasContext()
{
	return WorkflowExecutionContext::current() != nullptr;
}

void WorkflowReporter::info(const QString& text, const QString& source, const QString& scope, QVariantMap details)
{
	if (auto* context = WorkflowExecutionContext::current())
		context->info(text, source, scope, std::move(details));
}

void WorkflowReporter::warning(const QString& text, const QString& source, const QString& scope, QVariantMap details)
{
	if (auto* context = WorkflowExecutionContext::current())
		context->warning(text, source, scope, std::move(details));
}

void WorkflowReporter::error(const QString& text, const QString& source, const QString& scope, QVariantMap details)
{
	if (auto* context = WorkflowExecutionContext::current())
		context->error(text, source, scope, std::move(details));
}

void WorkflowReporter::message(SeverityLevel severity, const QString& text, const QString& source, const QString& code, const QString& scope, QVariantMap details)
{
	if (auto context = WorkflowExecutionContext::current()) {
		switch (severity) {
			case SeverityLevel::Info:
				context->info(text, source, scope, std::move(details));
				break;

			case SeverityLevel::Warning:
				context->warning(text, source, scope, std::move(details));
				break;

			case SeverityLevel::Error:
			case SeverityLevel::Fatal:
				context->error(text, source, scope, std::move(details));
				break;
		}
	}
}

void WorkflowReporter::setProgress(double value)
{
	if (auto* context = WorkflowExecutionContext::current())
		context->setProgress(value);
}

}
