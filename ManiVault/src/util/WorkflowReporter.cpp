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

void WorkflowReporter::info(QString text, QString location, QVariantMap details)
{
	if (auto context = WorkflowExecutionContext::current())
		context->info(std::move(text), std::move(location), std::move(details));
}

void WorkflowReporter::warning(QString text, QString location, QVariantMap details)
{
	if (auto context = WorkflowExecutionContext::current())
		context->warning(std::move(text), std::move(location), std::move(details));
}

void WorkflowReporter::error(QString text, QString location, QVariantMap details)
{
	if (auto context = WorkflowExecutionContext::current())
		context->error(std::move(text), std::move(location), std::move(details));
}

void WorkflowReporter::message(SeverityLevel severity, QString text, QString location, QVariantMap details)
{
	if (auto context = WorkflowExecutionContext::current()) {
		switch (severity) {
			case SeverityLevel::Info:
				context->info(std::move(text), std::move(location), std::move(details));
				break;

			case SeverityLevel::Warning:
				context->warning(std::move(text), std::move(location), std::move(details));
				break;

			case SeverityLevel::Error:
			case SeverityLevel::Fatal:
				context->error(std::move(text), std::move(location), std::move(details));
				break;
		}
	}
}

void WorkflowReporter::setProgress(double value)
{
	if (auto context = WorkflowExecutionContext::current())
		context->setProgress(value);
}

}
