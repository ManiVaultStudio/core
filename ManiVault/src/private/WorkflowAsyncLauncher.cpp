// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowAsyncLauncher.h"
#include "WorkflowRunner.h"

WorkflowAsyncLauncher::WorkflowAsyncLauncher(QObject* parent): QObject(parent)
{
}

void WorkflowAsyncLauncher::startWorkflowAsync(mv::util::WorkflowPlan workflowPlan, mv::Task* task, QObject* contextObject, FinishedCallback onFinished, FailedCallback onFailed)
{
	auto* thread = new QThread(contextObject);
	auto* runner = new WorkflowRunner(std::move(workflowPlan), task, std::move(onFinished), std::move(onFailed));

	runner->moveToThread(thread);

	QObject::connect(thread,
	                 &QThread::started,
	                 runner,
	                 &WorkflowRunner::run);

	if (task != nullptr) {
		QObject::connect(runner, &WorkflowRunner::progressChanged, task,[task](double progress) {
			task->setProgress(static_cast<float>(progress));
		    }, Qt::QueuedConnection);
	}

	QObject::connect(runner,
	                 &WorkflowRunner::finished,
	                 contextObject,
	                 [thread, runner, task, onFinished = std::move(onFinished)](const mv::util::WorkflowResult& result) mutable {
		                 if (task != nullptr) {
			                 task->setProgress(1.0f);
			                 task->setStatus(mv::Task::Status::Finished);
		                 }

		                 if (onFinished)
			                 onFinished(result);

		                 thread->quit();
		                 runner->deleteLater();
		                 thread->deleteLater();
	                 },
	                 Qt::QueuedConnection);

	QObject::connect(runner,
	                 &WorkflowRunner::failed,
	                 contextObject,
	                 [thread, runner, task, onFailed = std::move(onFailed)](const QString& error) mutable {
		                 //if (task != nullptr)
			                // task->setStatus(mv::Task::Status::);

		                 if (onFailed)
			                 onFailed(error);

		                 thread->quit();
		                 runner->deleteLater();
		                 thread->deleteLater();
	                 },
	                 Qt::QueuedConnection);

	QObject::connect(thread,
	                 &QThread::finished,
	                 thread,
	                 &QObject::deleteLater);

	if (task != nullptr) {
		task->setStatus(mv::Task::Status::Running);
		task->setProgress(0.0f);
	}

	thread->start();
}
