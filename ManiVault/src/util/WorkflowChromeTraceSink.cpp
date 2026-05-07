// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowChromeTraceSink.h"

#include <QMutexLocker>

namespace mv::util
{

WorkflowChromeTraceSink::WorkflowChromeTraceSink(const QString& filePath)
{
	_file.setFileName(filePath);
	_file.open(QIODevice::WriteOnly | QIODevice::Text);

	_stream.setDevice(&_file);

	_stream << "[\n";
}

WorkflowChromeTraceSink::~WorkflowChromeTraceSink()
{
	_stream << "{}]\n";
}

void WorkflowChromeTraceSink::trace(const WorkflowTraceEvent& event)
{
	writeEvent(event);
}

QString WorkflowChromeTraceSink::displayName(const WorkflowTraceEvent& event)
{
    switch (event._type) {
	    case WorkflowTraceEventType::WorkflowStarted:
	    case WorkflowTraceEventType::WorkflowFinished:
	    case WorkflowTraceEventType::WorkflowFailed:
	        return QString("Workflow: %1").arg(event._name);

	    case WorkflowTraceEventType::StageStarted:
	    case WorkflowTraceEventType::StageFinished:
	    case WorkflowTraceEventType::StageFailed:
	        return QString("Stage: %1").arg(event._name);

	    case WorkflowTraceEventType::JobStarted:
	    case WorkflowTraceEventType::JobFinished:
	    case WorkflowTraceEventType::JobFailed:
	        return QString("Job: %1").arg(event._name);

	    case WorkflowTraceEventType::GuiDispatchRequested:
	    case WorkflowTraceEventType::GuiDispatchEntered:
	    case WorkflowTraceEventType::GuiDispatchFinished:
	        return QString("GUI: %1").arg(event._name);

	    case WorkflowTraceEventType::ParallelJobSubmitted:
	    case WorkflowTraceEventType::ParallelJobStarted:
	    case WorkflowTraceEventType::ParallelJobFinished:
	    case WorkflowTraceEventType::ParallelJobFailed:
	        return QString("Parallel: %1").arg(event._name);
    }

    return event._name;
}

void WorkflowChromeTraceSink::writeEvent(const WorkflowTraceEvent& event)
{
    QMutexLocker locker(&_mutex);

    const auto threadId = reinterpret_cast<quintptr>(event._threadId);

    QString phase = "i";

    switch (event._type) {
	    case WorkflowTraceEventType::JobStarted:
	    case WorkflowTraceEventType::StageStarted:
	    case WorkflowTraceEventType::WorkflowStarted:
	        phase = "B";
	        break;

	    case WorkflowTraceEventType::JobFinished:
	    case WorkflowTraceEventType::StageFinished:
	    case WorkflowTraceEventType::WorkflowFinished:
	        phase = "E";
	        break;

	    default:
	        phase = "i";
	        break;
    }

    _stream
        << "{"
        << "\"name\":\"" << displayName(event) << "\","
        << "\"ph\":\"" << phase << "\","
        << "\"ts\":" << (event._timestampNs / 1000) << ","
        << "\"pid\":1,"
        << "\"tid\":" << threadId;

    if (!event._metadata.isEmpty()) {

        _stream << ",\"args\":{";

        bool first = true;

        for (auto it = event._metadata.begin(); it != event._metadata.end(); ++it) {

            if (!first)
                _stream << ",";

            _stream
                << "\"" << it.key() << "\":"
                << "\"" << it.value().toString() << "\"";

            first = false;
        }

        _stream << "}";
    }

    _stream << "},\n";

    _stream.flush();
}

}
