// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "AbstractWorkflowTraceSink.h"

#include <QString>

namespace mv::util
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
     * Writes a workflow trace event to the trace file in Chrome trace format.
     * @param event The workflow trace event to write to the file.
     */
    void writeEvent(const WorkflowTraceEvent& event);

private:
    QFile           _file;      /** File object for writing the trace events */
    QTextStream     _stream;    /** Text stream for writing trace events to the file */
    QMutex          _mutex;     /** Mutex to ensure thread-safe access to the file when writing trace events */
};

}