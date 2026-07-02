// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

namespace mv::workflow
{

/**
 * @brief Options for controlling the batching of workflow tasks.
 *
 * This struct defines options for controlling how many datasets or data blocks
 * are processed in parallel during workflow execution. It provides conservative
 * defaults based on the number of hardware threads available, but can be
 * customized to optimize performance for specific workloads.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
struct CORE_EXPORT WorkflowBatchingOptions
{
    std::size_t datasetLoadingBatchSize     = conservativeDatasetsSerializationBatchSize();     /**< Number of datasets loaded in parallel */
    std::size_t datasetSavingBatchSize      = conservativeDatasetsSerializationBatchSize();     /**< Number of datasets saved in parallel */
    std::size_t dataBlockEncodingBatchSize  = conservativeBlockSerializationBatchSize();        /**< Number of data blocks encoded in parallel */
    std::size_t dataBlockDecodingBatchSize  = conservativeBlockSerializationBatchSize();        /**< Number of data blocks decoded in parallel */

    /**
     * @brief Returns a conservative batch size for dataset serialization.
     * @return Suggested dataset serialization batch size.
     */
    [[nodiscard]] static std::size_t conservativeDatasetsSerializationBatchSize();

    /**
     * @brief Returns a conservative batch size for data block serialization.
     * @return Suggested data block serialization batch size.
     */
    [[nodiscard]] static std::size_t conservativeBlockSerializationBatchSize();
};

}
