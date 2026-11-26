// SPDX-License-Identifier: LGPL-3.0-or-later

#include "DenseStorageBackendConversionAction.h"

#include <limits>

namespace mv::gui
{

DenseStorageBackendConversionAction::DenseStorageBackendConversionAction(QObject* parent, const std::uint64_t rows, const std::uint64_t columns, const std::uint32_t currentElementTypeIndex) :
    StorageBackendConversionAction(parent, "Dense storage", currentElementTypeIndex),
    _rows(rows),
    _columns(columns)
{
    connect(this, &StorageBackendConversionAction::optionsChanged, this, &DenseStorageBackendConversionAction::updateCanApply);
    updateCanApply();
    moveElementTypeAnalysisActionToEnd();
}

void DenseStorageBackendConversionAction::updateCanApply()
{
    const auto elementSize = getTargetElementSize();
    const auto invalidDimensions = _rows != 0 && _columns > (std::numeric_limits<std::uint64_t>::max)() / _rows;
    const auto elementCount = invalidDimensions ? 0 : _rows * _columns;
    _canApply = elementSize && !invalidDimensions && *elementSize != 0 && elementCount <= (std::numeric_limits<std::size_t>::max)() / *elementSize;
}

pointdata::storage::StorageBackendType DenseStorageBackendConversionAction::getTargetBackendType() const
{
    return pointdata::storage::StorageBackendType::Dense;
}

pointdata::storage::StorageBackendConfigurationOptionsPtr DenseStorageBackendConversionAction::getConversionOptions() const
{
    auto options = std::make_shared<pointdata::storage::StorageBackendConfigurationOptions>();
    options->targetElementTypeIndex = getTargetElementTypeIndex();
    return options;
}

bool DenseStorageBackendConversionAction::canApply() const
{
    return _canApply;
}

}
