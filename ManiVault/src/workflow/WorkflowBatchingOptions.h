// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "AbstractWorkflowTraceSink.h"

#include <thread>

namespace mv::workflow
{

struct CORE_EXPORT WorkflowBatchingOptions
{
    std::size_t _datasetLoadingBatchSize    = conservativeDatasetsSerializationBatchSize();     /** Number of datasets to load in parallel when loading multiple datasets. */
    std::size_t _datasetSavingBatchSize     = conservativeDatasetsSerializationBatchSize();     /** Number of datasets to save in parallel when saving multiple datasets. */
    std::size_t _dataBlockEncodingBatchSize = conservativeBlockSerializationBatchSize();        /** Number of data blocks to encode in parallel when encoding multiple data blocks. */
    std::size_t _dataBlockDecodingBatchSize = conservativeBlockSerializationBatchSize();        /** Number of data blocks to decode in parallel when decoding multiple data blocks. */

    /** Returns a conservative batch size based on the number of hardware threads available. This is useful for determining how many tasks to run in parallel without overwhelming the system. */
    static std::size_t conservativeDatasetsSerializationBatchSize();

    /** Returns a conservative batch size for data block operations based on the number of hardware threads available. This is useful for determining how many data block encoding/decoding tasks to run in parallel without overwhelming the system. */
    static std::size_t conservativeBlockSerializationBatchSize();
};

} // namespace mv::util