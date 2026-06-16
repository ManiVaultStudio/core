// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "WorkflowBatchingOptions.h"

namespace mv::workflow
{

std::size_t WorkflowBatchingOptions::conservativeDatasetsSerializationBatchSize()
{
	const auto cores = std::max(1u, std::thread::hardware_concurrency());

	return std::clamp<std::size_t>(static_cast<std::size_t>(cores / 8), 2, 8);
}

std::size_t WorkflowBatchingOptions::conservativeBlockSerializationBatchSize()
{
	const auto cores = std::max(1u, std::thread::hardware_concurrency());

	return std::clamp<std::size_t>(static_cast<std::size_t>(cores / 6), 2, 8);
}

}
