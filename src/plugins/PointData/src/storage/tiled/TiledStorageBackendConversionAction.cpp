// SPDX-License-Identifier: LGPL-3.0-or-later

#include "TiledStorageBackendConversionAction.h"

#include "../interface/StorageBackendMemory.h"

#include <algorithm>
#include <limits>

namespace mv::gui
{

TiledStorageBackendConversionAction::TiledStorageBackendConversionAction(QObject* parent, const std::uint64_t rows, const std::uint64_t columns, const std::size_t elementSize, const pointdata::storage::StorageBackendConfiguration& configuration, const bool initializeFromConfiguration, const bool materializesFullMatrix, const std::uint32_t currentElementTypeIndex) :
    TiledStorageBackendConfigurationAction(parent, rows, columns, elementSize, configuration, initializeFromConfiguration, currentElementTypeIndex),
    _rows(rows),
    _columns(columns),
    _sourceElementTypeIndex(currentElementTypeIndex),
    _hasTiledSource(initializeFromConfiguration)
{
    enableConversionPerformanceOptions(configuration.conversionWorkerCount, configuration.conversionChunkTileRows);

    if (!materializesFullMatrix)
    {
        moveElementTypeAnalysisActionToEnd();
        return;
    }

    _checksMaterialization = true;

    connect(this, &StorageBackendConversionAction::optionsChanged, this, &TiledStorageBackendConversionAction::updateMemoryObservation);
    updateMemoryObservation();
    moveElementTypeAnalysisActionToEnd();
}

void TiledStorageBackendConversionAction::updateMemoryObservation()
{
    if (!_checksMaterialization)
        return;

    _canApply = true;
    const auto invalidDimensions = _rows != 0 && _columns > (std::numeric_limits<std::uint64_t>::max)() / _rows;
    const auto targetElementSize = getTargetElementSize();
    const auto elementCount = invalidDimensions ? 0 : _rows * _columns;
    const auto invalidAllocation = invalidDimensions || !targetElementSize || *targetElementSize == 0 || elementCount > (std::numeric_limits<std::uint64_t>::max)() / *targetElementSize;
    if (invalidAllocation)
    {
        _canApply = false;
        return;
    }

    auto estimatedBytes = elementCount * *targetElementSize;
    const auto configuration = getConfiguration();
    if (_hasTiledSource && configuration.packedTiles && getTargetElementTypeIndex() == _sourceElementTypeIndex)
    {
        const auto tileRows = std::min<std::uint64_t>(_rows, configuration.rowTileSize);
        const auto tileColumns = std::min<std::uint64_t>(_columns, configuration.columnTileSize);
        if (tileRows != 0 && tileColumns > std::numeric_limits<std::uint64_t>::max() / tileRows)
        {
            _canApply = false;
            return;
        }

        const auto tileElements = tileRows * tileColumns;
        if (tileElements != 0 && *targetElementSize > std::numeric_limits<std::uint64_t>::max() / tileElements)
        {
            _canApply = false;
            return;
        }

        const auto tileBytes = tileElements * *targetElementSize;
        if (tileBytes > std::numeric_limits<std::uint64_t>::max() / 2)
        {
            _canApply = false;
            return;
        }

        const auto tileWorkingBytes = tileBytes * 2;
        const auto workerCount = std::clamp<std::size_t>(configuration.conversionWorkerCount, 1, 32);
        if (tileWorkingBytes != 0 && workerCount > std::numeric_limits<std::uint64_t>::max() / tileWorkingBytes)
        {
            _canApply = false;
            return;
        }

        estimatedBytes = tileWorkingBytes * workerCount;
    }
    const auto availableRamBytes = getAvailablePhysicalMemoryBytes();
    if (!availableRamBytes)
    {
        return;
    }

    _canApply = estimatedBytes <= *availableRamBytes;
}

bool TiledStorageBackendConversionAction::canApply() const
{
    return _canApply;
}

}
