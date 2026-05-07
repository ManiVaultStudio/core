// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "AbstractWorkflowTraceSink.h"

#include <QString>

namespace mv::util
{

class WorkflowConsoleTraceSink : public AbstractWorkflowTraceSink
{
public:
    void trace(const WorkflowTraceEvent& event) override;

private:
    static QString typeString(const WorkflowTraceEvent& event);

    static QString timestampString(const WorkflowTraceEvent& event);

    static QString threadString(const WorkflowTraceEvent& event);

    static QString metadataString(const WorkflowTraceEvent& event);
};

}