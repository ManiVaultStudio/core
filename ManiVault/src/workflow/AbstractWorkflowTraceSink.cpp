// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AbstractWorkflowTraceSink.h"
#include "WorkflowConsoleTraceSink.h"
#include "WorkflowChromeTraceSink.h"

namespace mv::workflow
{

std::int64_t AbstractWorkflowTraceSink::currentTimestampNs()
{
	return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

SharedWorkflowTraceSink createWorkflowTraceSink(WorkflowTraceSinkType type, const QString& outputPath)
{
	switch (type) {
		case WorkflowTraceSinkType::None:
			return nullptr;

		case WorkflowTraceSinkType::Console:
			return std::make_shared<WorkflowConsoleTraceSink>();

		case WorkflowTraceSinkType::ChromeJson:
			return std::make_shared<WorkflowChromeTraceSink>(outputPath);
	}

	return nullptr;
}

}
