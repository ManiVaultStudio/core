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
 * A trace sink that writes workflow trace events to the console.
 *
 * @author T. Kroes (BioVault)
 */
class CORE_EXPORT WorkflowConsoleTraceSink : public AbstractWorkflowTraceSink
{
public:

	/**
     * Writes a workflow trace event to the console in a human-readable format.
     * @param event The workflow trace event to write to the console.
	 */
	void trace(const WorkflowTraceEvent& event) override;

private:

    /**
     * Converts a workflow trace event type to a human-readable string.
     * @param event The workflow trace event for which to get the type string.
     * @return A human-readable string representing the type of the workflow trace event.
     */
    static QString typeString(const WorkflowTraceEvent& event);

    /**
     * Converts the timestamp of a workflow trace event to a human-readable string.
     * @param event The workflow trace event for which to get the timestamp string.
     * @return A human-readable string representing the timestamp of the workflow trace event.
     */
    static QString timestampString(const WorkflowTraceEvent& event);

    /**
     * Converts the thread identifier of a workflow trace event to a human-readable string.
     * @param event The workflow trace event for which to get the thread identifier string.
     * @return A human-readable string representing the thread identifier of the workflow trace event.
     */
    static QString threadString(const WorkflowTraceEvent& event);

    /**
     * Converts the metadata of a workflow trace event to a human-readable string.
     * @param event The workflow trace event for which to get the metadata string.
     * @return A human-readable string representing the metadata of the workflow trace event.
     */
    static QString metadataString(const WorkflowTraceEvent& event);
};

}