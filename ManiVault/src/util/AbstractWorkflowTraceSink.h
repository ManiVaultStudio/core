// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowTraceEvent.h"

namespace mv::util
{
/**
 * Abstract base class for workflow trace sinks that can receive and process workflow trace events.
 *
 * @author T. Kroes (BioVault)
 */
class CORE_EXPORT AbstractWorkflowTraceSink
{
public:
    virtual ~AbstractWorkflowTraceSink() = default;

    /**
     * Traces a workflow event by processing the provided WorkflowTraceEvent object.
     * @param event The WorkflowTraceEvent object containing details about the event to be traced.
     */
    virtual void trace(const WorkflowTraceEvent& event) = 0;

    /** Flushes any buffered trace data to the underlying storage or output medium. */
    virtual void flush() {}

    /**
     * Returns the current timestamp in nanoseconds since an arbitrary epoch (steady clock).
     * @return The current timestamp in nanoseconds.
     */
    static std::int64_t currentTimestampNs();
};

}