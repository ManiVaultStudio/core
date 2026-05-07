// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QString>

namespace mv::util
{

/** Event types emitted during workflow execution for tracing and debugging purposes */
enum class WorkflowTraceEventType
{
    /** Workflow execution events */
    WorkflowStarted,            /** Emitted when a workflow starts execution, before any stages or jobs have started */
    WorkflowFinished,           /** Emitted when a workflow finishes execution, after all stages and jobs have completed */
    WorkflowFailed,             /** Emitted when a workflow fails execution, after all stages and jobs have completed */

    /** Stage execution events */
    StageStarted,               /** Emitted when a stage starts execution */
    StageFinished,              /** Emitted when a stage finishes execution */
    StageFailed,                /** Emitted when a stage fails execution */

    /** Job execution events */
    JobStarted,                 /** Emitted when a job starts execution */
    JobFinished,                /** Emitted when a job finishes execution */
    JobFailed,                  /** Emitted when a job fails execution */

    /** GUI dispatch events */
    GuiDispatchRequested,       /** Emitted when a job requests dispatch of a function to the GUI thread */
    GuiDispatchEntered,         /** Emitted when execution enters a function dispatched to the GUI thread */
    GuiDispatchFinished,        /** Emitted when execution finishes a function dispatched to the GUI thread */

    /** Parallel job events */
	ParallelJobSubmitted,       /** Emitted when a job is submitted for parallel execution, before it starts executing */
    ParallelJobStarted,         /** Emitted when a parallel job starts execution */
    ParallelJobFinished,        /** Emitted when a parallel job finishes execution */
    ParallelJobFailed,          /** Emitted when a parallel job fails execution */
};

/** Struct representing a single trace event during workflow execution */
struct WorkflowTraceEvent
{
    WorkflowTraceEventType      _type;                  /** Type of the trace event, see WorkflowTraceEventType */
    QString                     _name;                  /** Name of the workflow, stage, or job associated with the event */
    QUuid                       _contextId;             /** Unique identifier for the context in which the event occurred */
    QUuid                       _parentContextId;       /** Unique identifier for the parent context, if any */
    Qt::HANDLE                  _threadId = nullptr;    /** Identifier of the thread in which the event occurred */
    qint64                      _timestampNs = 0;       /** Timestamp of the event in nanoseconds since epoch */        
    QVariantMap                 _metadata;              /** Optional additional metadata associated with the event */
};

} // namespace mv::util