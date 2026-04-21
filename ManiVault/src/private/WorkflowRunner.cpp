// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowRunner.h"
#include "WorkflowPlanExecutor.h"

WorkflowRunner::WorkflowRunner(mv::util::WorkflowPlan workflowPlan, mv::Task* task, FinishedCallback onFinished, FailedCallback onFailed, QObject* parent):
	QObject(parent),
	_workflowPlan(std::move(workflowPlan)),
	_task(task),
	_onFinished(std::move(onFinished)),
	_onFailed(std::move(onFailed))
{
}

void WorkflowRunner::run()
{
	//try {
	//	WorkflowPlanExecutor executor;

	//	auto result = executor.execute(
	//		_workflowPlan,
	//		nullptr,
	//		[this](double progress) {
	//			emit progressChanged(progress);
	//		});

	//	emit finished(result);
	//}
	//catch (const std::exception& e) {
	//	emit failed(QString::fromUtf8(e.what()));
	//}
	//catch (...) {
	//	emit failed("Unknown workflow error");
	//}
}
