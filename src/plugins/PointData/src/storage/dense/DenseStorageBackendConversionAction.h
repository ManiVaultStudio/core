// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "../interface/StorageBackendConversionAction.h"

namespace mv::gui
{

/**
 * @brief Provides controls for converting a backend to dense storage.
 *
 * Checks that the dense matrix dimensions and byte size fit the process's
 * addressable limits before allowing conversion.
 *
 * @author Thomas Kroes
 */
class POINTDATA_EXPORT DenseStorageBackendConversionAction final : public StorageBackendConversionAction
{
public:
    /** Constructs dense conversion controls for a matrix with the supplied shape and current element type. */
    DenseStorageBackendConversionAction(QObject* parent, std::uint64_t rows, std::uint64_t columns, std::uint32_t currentElementTypeIndex);

    /** Returns the dense backend type. */
    pointdata::storage::StorageBackendType getTargetBackendType() const override;
    /** Returns no backend-specific options for dense storage. */
    pointdata::storage::StorageBackendConfigurationOptionsPtr getConversionOptions() const override;
    /** Returns false when the dense allocation exceeds the process's addressable limits. */
    bool canApply() const override;

private:
    void updateCanApply();

    std::uint64_t _rows = 0;                    /**< Matrix row count used by the allocation estimate. */
    std::uint64_t _columns = 0;                 /**< Matrix column count used by the allocation estimate. */
    bool _canApply = true;                      /**< Whether the dense allocation fits addressable limits. */
};

}
