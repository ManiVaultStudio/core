// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "AbstractWorkflowPlanExecutor.h"

#include <QUrlQuery>

namespace mv::util
{

AbstractWorkflowPlanExecutor::AbstractWorkflowPlanExecutor(QObject* parent):
	QObject(parent)
{
}

SharedWorkflowResult AbstractWorkflowPlanExecutor::waitForAsync(WorkflowResultFuture::State& state)
{
    auto result = state.future.get();

    state.rethrowExceptionIfAny();

    return result;
}

void AbstractWorkflowPlanExecutor::installNotificationLinkHandler()
{
    mv::help().addNotificationLinkHandler("workflow/results", [](const QUrl& url) {

    	QUrlQuery query(url);

    	const auto workflowResultId = query.queryItemValue("workflowResultId", QUrl::FullyDecoded);

        if (workflowResultId.isEmpty()) {
            qDebug() << "No workflowResultId query parameter found";
            return;
        }

        const auto queriedWorkflowMessageLevels = query.queryItemValue("levels", QUrl::FullyDecoded);

        SeverityLevels severityLevels;

        if (!queriedWorkflowMessageLevels.isEmpty()) {
            severityLevels.clear();

            for (const auto& workflowMessageLevel : queriedWorkflowMessageLevels.split(",")) {
                severityLevels << getSeverityLevel(workflowMessageLevel);
            }
        }

        const auto result = WorkflowResultRegistry::instance().get(QUuid(workflowResultId));

        if (!result) {
            qDebug() << "Workflow result not found for ID:" << workflowResultId;
            return;
        }

        WorkflowResultDialog dialog(result, severityLevels);

        dialog.exec();
    });
}

SharedWorkflowExecutionContext AbstractWorkflowPlanExecutor::requireContext(const SharedWorkflowExecutionContext& context, const char* where)
{
	if (!context || !context->isValid()) {
		throw std::runtime_error(
			QString("Invalid workflow execution context in %1").arg(where).toStdString()
		);
	}

	return context;
}

void AbstractWorkflowPlanExecutor::trace(const SharedWorkflowExecutionContext& context, WorkflowTraceEvent event)
{
    if (auto state = context->getState()) {
        if (auto traceSink = state->getTraceSink()) {
            event._threadId     = QThread::currentThreadId();
            event._timestampNs  = AbstractWorkflowTraceSink::currentTimestampNs();

            traceSink->trace(event);
        }
    }
}

}

