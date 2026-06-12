// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowChromeTraceSink.h"

#include <QMutexLocker>

namespace mv::workflow
{

WorkflowChromeTraceSink::WorkflowChromeTraceSink(const QString& filePath)
{
	_file.setFileName(filePath);
    _file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);

	_stream.setDevice(&_file);

    _stream << "[\n";
}

WorkflowChromeTraceSink::~WorkflowChromeTraceSink()
{
    QMutexLocker locker(&_mutex);


    _stream << "\n]\n";
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
            return QString("GUI requested: %1").arg(event._name);

        case WorkflowTraceEventType::GuiDispatchEntered:
            return QString("GUI entered: %1").arg(event._name);

        case WorkflowTraceEventType::GuiDispatchFinished:
            return QString("GUI finished: %1").arg(event._name);

	    case WorkflowTraceEventType::ParallelJobSubmitted:
	    case WorkflowTraceEventType::ParallelJobStarted:
	    case WorkflowTraceEventType::ParallelJobFinished:
	    case WorkflowTraceEventType::ParallelJobFailed:
	        return QString("Parallel: %1").arg(event._name);

        case WorkflowTraceEventType::PendingAsyncWorkWaitStarted:
        case WorkflowTraceEventType::PendingAsyncWorkWaitFinished:
            return QString("Async wait: %1").arg(event._name);

        case WorkflowTraceEventType::PendingAsyncWorkItemStarted:
        case WorkflowTraceEventType::PendingAsyncWorkItemFinished:
            return QString("Async item: %1").arg(event._name);

        case WorkflowTraceEventType::ParallelStageStarted:
        case WorkflowTraceEventType::ParallelStageFinished:
        case WorkflowTraceEventType::ParallelStageFailed:
            return QString("Parallel stage: %1").arg(event._name);
    }

    return event._name;
}

QString WorkflowChromeTraceSink::phaseString(const WorkflowTraceEvent& event)
{
    switch (event._type) {
	    case WorkflowTraceEventType::WorkflowStarted:
	    case WorkflowTraceEventType::StageStarted:
	    case WorkflowTraceEventType::JobStarted:
	    case WorkflowTraceEventType::ParallelJobStarted:
	    case WorkflowTraceEventType::PendingAsyncWorkWaitStarted:
	    case WorkflowTraceEventType::PendingAsyncWorkItemStarted:
	    case WorkflowTraceEventType::ParallelStageStarted:
	        return "B";

	    case WorkflowTraceEventType::WorkflowFinished:
	    case WorkflowTraceEventType::WorkflowFailed:
	    case WorkflowTraceEventType::StageFinished:
	    case WorkflowTraceEventType::StageFailed:
	    case WorkflowTraceEventType::JobFinished:
	    case WorkflowTraceEventType::JobFailed:
	    case WorkflowTraceEventType::ParallelJobFinished:
	    case WorkflowTraceEventType::ParallelJobFailed:
	    case WorkflowTraceEventType::PendingAsyncWorkWaitFinished:
	    case WorkflowTraceEventType::PendingAsyncWorkItemFinished:
	    case WorkflowTraceEventType::PendingAsyncWorkItemFailed:
	    case WorkflowTraceEventType::ParallelStageFinished:
	    case WorkflowTraceEventType::ParallelStageFailed:
	        return "E";

	    case WorkflowTraceEventType::GuiDispatchRequested:
	    case WorkflowTraceEventType::GuiDispatchEntered:
	    case WorkflowTraceEventType::GuiDispatchFinished:
	    case WorkflowTraceEventType::ParallelJobSubmitted:
	        return "i";
    }

    return "i";
}

void WorkflowChromeTraceSink::writeEvent(const WorkflowTraceEvent& event)
{
    QMutexLocker locker(&_mutex);

    if (!_firstEvent)
        _stream << ",\n";

    _firstEvent = false;

    _stream << "{";

    // write fields...
    _stream
        << "\"name\":\"" << displayName(event) << "\","
        << "\"ph\":\"" << phaseString(event) << "\","
        << "\"ts\":" << event._timestampNs / 1000 << ","
        << "\"pid\":1,"
        << "\"tid\":" << reinterpret_cast<quintptr>(event._threadId);

    if (!event._metadata.isEmpty()) {
        _stream << ",\"args\":{";

        bool firstArg = true;

        for (auto it = event._metadata.begin(); it != event._metadata.end(); ++it) {
            if (!firstArg)
                _stream << ",";

            firstArg = false;

            _stream
                << "\"" << it.key() << "\":"
                << "\"" << it.value().toString() << "\"";
        }

        _stream << "}";
    }

    _stream << "}";

    _stream.flush();
}

}
