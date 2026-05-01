// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "AbstractWorkflowPlanExecutor.h"

namespace mv::util
{

QElapsedTimer AbstractWorkflowPlanExecutor::getElapsedTimer() const
{
	return _elapsedTimer;
}

void AbstractWorkflowPlanExecutor::beginTimer()
{
	_elapsedTimer.start();
}

void AbstractWorkflowPlanExecutor::endTimer(SharedWorkflowResult result)
{
    Q_ASSERT(result);

    if (result)
		result->setDuration(static_cast<std::uint64_t>(_elapsedTimer.elapsed()));
}

}

