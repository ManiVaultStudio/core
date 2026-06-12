// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "AbstractWorkflowTraceSink.h"

#include <QString>

namespace mv::workflow
{

/**
 * A trace sink that writes workflow trace events to a Chrome trace file.
 *
 * @author T. Kroes (BioVault)
 */
class CORE_EXPORT WorkflowChromeTraceSink : public AbstractWorkflowTraceSink
{
public:

    /**
     * Constructs a WorkflowChromeTraceSink that writes to the specified file path.
     * @param filePath The path of the file to write the trace events to.
     */
    WorkflowChromeTraceSink(const QString& filePath);

    /* Destructor that finalizes the trace file. */
    ~WorkflowChromeTraceSink() override;

    /**
     * Writes a workflow trace event to the trace file in Chrome trace format.
     * @param event The workflow trace event to write to the file.
     */
    void trace(const WorkflowTraceEvent& event) override;

private:

    /**
     * Converts a WorkflowTraceEvent to a human-readable display name for use in the trace file.
     * @param event The workflow trace event to convert to a display name.
     * @return A QString representing the display name for the event, which may include the event type and relevant metadata for easier identification in the trace viewer.
     */
    static QString displayName(const WorkflowTraceEvent& event);

    /**
     * Converts a WorkflowTraceEvent to a string representation of the event phase for use in the trace file.
     * @param event The workflow trace event to convert to a phase string.
     * @return A QString representing the phase of the event (e.g., "B" for begin, "E" for end, "X" for complete) according to the Chrome trace format specifications.
     */
    static QString phaseString(const WorkflowTraceEvent& event);

    /**
     * Writes a workflow trace event to the trace file in Chrome trace format.
     * @param event The workflow trace event to write to the file.
     */
    void writeEvent(const WorkflowTraceEvent& event);

private:
    QFile           _file;                  /** File object for writing the trace events */
    QTextStream     _stream;                /** Text stream for writing trace events to the file */
    QMutex          _mutex;                 /** Mutex to ensure thread-safe access to the file when writing trace events */
    bool            _firstEvent = true;     /** Flag to indicate if the current event is the first event in the trace file */
};

}