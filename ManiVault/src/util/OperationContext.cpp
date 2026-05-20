// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "OperationContext.h"

namespace mv::util
{

void OperationContext::reportWarning(QString scope, QString message)
{
	addIssue(OperationSeverity::Warning, std::move(scope), std::move(message));
}

void OperationContext::reportError(QString scope, QString message)
{
	addIssue(OperationSeverity::Error, std::move(scope), std::move(message));
}

void OperationContext::reportFatal(QString scope, QString message)
{
	addIssue(OperationSeverity::Fatal, std::move(scope), std::move(message));

	_abortRequested.store(true, std::memory_order_relaxed);
}

bool OperationContext::hasWarnings() const
{
	return hasSeverity(OperationSeverity::Warning);
}

bool OperationContext::hasErrors() const
{
	return hasSeverity(OperationSeverity::Error) || hasSeverity(OperationSeverity::Fatal);
}

bool OperationContext::hasFatalError() const
{
	return hasSeverity(OperationSeverity::Fatal);
}

QList<OperationIssue> OperationContext::issues() const
{
	QMutexLocker lock(&_mutex);

	return _issues;
}

QString OperationContext::firstErrorMessage() const
{
	QMutexLocker lock(&_mutex);

	for (const auto& issue : _issues) {
		if (issue.severity == OperationSeverity::Error || issue.severity == OperationSeverity::Fatal)
			return issue.message;
	}

	return {};
}

QString OperationContext::combinedErrorMessage() const
{
	QMutexLocker lock(&_mutex);

	QStringList messages;

	for (const auto& issue : _issues) {
		if (issue.severity == OperationSeverity::Error || issue.severity == OperationSeverity::Fatal) {

			if (!issue.scope.isEmpty())
				messages << QString("[%1] %2").arg(issue.scope, issue.message);
			else
				messages << issue.message;
		}
	}

	return messages.join('\n');
}

void OperationContext::requestAbort()
{
	_abortRequested.store(true, std::memory_order_relaxed);
}

bool OperationContext::isAbortRequested() const
{
	return _abortRequested.load(std::memory_order_relaxed);
}

void OperationContext::addIssue(OperationSeverity severity, QString scope, QString message)
{
	{
		QMutexLocker lock(&_mutex);

		_issues.append(OperationIssue{
			severity,
			std::move(scope),
			std::move(message)
		});
	}

	if (severity == OperationSeverity::Fatal)
		_abortRequested.store(true, std::memory_order_relaxed);
}

bool OperationContext::hasSeverity(OperationSeverity severity) const
{
	QMutexLocker lock(&_mutex);

	for (const auto& issue : _issues) {
		if (issue.severity == severity)
			return true;
	}

	return false;
}

}
