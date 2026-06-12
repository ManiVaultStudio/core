// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowConsoleTraceSink.h"

namespace mv::workflow
{
void WorkflowConsoleTraceSink::trace(const WorkflowTraceEvent& event)
{
	qDebug().noquote() << QString("[%1] [%2] [%3]")
	                      .arg(timestampString(event))
	                      .arg(threadString(event))
	                      .arg(typeString(event))
		<< event._name
		<< metadataString(event);
}

QString WorkflowConsoleTraceSink::typeString(const WorkflowTraceEvent& event)
{
	switch (event._type) {
		case WorkflowTraceEventType::WorkflowStarted:
			return "WorkflowStarted";

		case WorkflowTraceEventType::WorkflowFinished:
			return "WorkflowFinished";

		case WorkflowTraceEventType::StageStarted:
			return "StageStarted";

		case WorkflowTraceEventType::StageFinished:
			return "StageFinished";

		case WorkflowTraceEventType::JobStarted:
			return "JobStarted";

		case WorkflowTraceEventType::JobFinished:
			return "JobFinished";

		case WorkflowTraceEventType::JobFailed:
			return "JobFailed";

		case WorkflowTraceEventType::GuiDispatchRequested:
			return "GuiDispatchRequested";

		case WorkflowTraceEventType::GuiDispatchEntered:
			return "GuiDispatchEntered";
	}

	return "Unknown";
}

QString WorkflowConsoleTraceSink::timestampString(const WorkflowTraceEvent& event)
{
	return QString::number(event._timestampNs);
}

QString WorkflowConsoleTraceSink::threadString(const WorkflowTraceEvent& event)
{
	return QString("thread=%1").arg(reinterpret_cast<quintptr>(event._threadId));
}

QString WorkflowConsoleTraceSink::metadataString(const WorkflowTraceEvent& event)
{
	QStringList parts;

	for (auto it = event._metadata.begin(); it != event._metadata.end(); ++it)
		parts << QString("%1=%2").arg(it.key()).arg(it.value().toString());

	return parts.join(", ");
}

}
