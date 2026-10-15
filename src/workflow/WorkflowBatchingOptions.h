// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

namespace mv::workflow
{

/**
 * @brief Configures workflow serialization batch sizes.
 *
 * These options bound how many datasets or data blocks may be processed in
 * parallel during loading, saving, encoding, and decoding.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
struct CORE_EXPORT WorkflowBatchingOptions
{
    std::size_t datasetLoadingBatchSize     = conservativeDatasetsSerializationBatchSize();     /**< Number of datasets to load in parallel. */
    std::size_t datasetSavingBatchSize      = conservativeDatasetsSerializationBatchSize();     /**< Number of datasets to save in parallel. */
    std::size_t dataBlockEncodingBatchSize  = conservativeBlockSerializationBatchSize();        /**< Number of data blocks to encode in parallel. */
    std::size_t dataBlockDecodingBatchSize  = conservativeBlockSerializationBatchSize();        /**< Number of data blocks to decode in parallel. */

    /**
     * @brief Returns the conservative dataset batch size.
     * @return Number of datasets to process concurrently.
     */
    [[nodiscard]] static std::size_t conservativeDatasetsSerializationBatchSize();

    /**
     * @brief Returns the conservative data-block batch size.
     * @return Number of data blocks to process concurrently.
     */
    [[nodiscard]] static std::size_t conservativeBlockSerializationBatchSize();
};

}
