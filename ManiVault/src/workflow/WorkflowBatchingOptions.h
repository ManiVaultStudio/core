// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

namespace mv::workflow
{

/** Options for controlling the batching behavior of workflow execution. These options determine how many datasets or data blocks are processed in parallel during various stages of workflow execution, such as loading, saving, encoding, and decoding. Adjust these options based on the expected workload and system capabilities to achieve optimal performance without overwhelming system resources. */
struct CORE_EXPORT WorkflowBatchingOptions
{
    std::size_t datasetLoadingBatchSize     = conservativeDatasetsSerializationBatchSize();     /** Number of datasets to load in parallel when loading multiple datasets. */
    std::size_t datasetSavingBatchSize      = conservativeDatasetsSerializationBatchSize();     /** Number of datasets to save in parallel when saving multiple datasets. */
    std::size_t dataBlockEncodingBatchSize  = conservativeBlockSerializationBatchSize();        /** Number of data blocks to encode in parallel when encoding multiple data blocks. */
    std::size_t dataBlockDecodingBatchSize  = conservativeBlockSerializationBatchSize();        /** Number of data blocks to decode in parallel when decoding multiple data blocks. */

    /** Returns a conservative batch size based on the number of hardware threads available. This is useful for determining how many tasks to run in parallel without overwhelming the system. */
    static std::size_t conservativeDatasetsSerializationBatchSize();

    /** Returns a conservative batch size for data block operations based on the number of hardware threads available. This is useful for determining how many data block encoding/decoding tasks to run in parallel without overwhelming the system. */
    static std::size_t conservativeBlockSerializationBatchSize();
};

}