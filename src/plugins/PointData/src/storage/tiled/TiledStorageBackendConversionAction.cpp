// SPDX-License-Identifier: LGPL-3.0-or-later

#include "TiledStorageBackendConversionAction.h"

#include "PointDataTiledStorage.h"
#include "../interface/StorageBackendMemory.h"

#include <util/Miscellaneous.h>

#include <algorithm>
#include <limits>

namespace mv::gui
{

TiledStorageBackendConversionAction::TiledStorageBackendConversionAction(QObject* parent, const std::uint64_t rows, const std::uint64_t columns, const std::size_t elementSize, const pointdata::storage::StorageBackendConfiguration& configuration, const bool initializeFromConfiguration, const bool materializesFullMatrix, const std::uint32_t currentElementTypeIndex) :
    TiledStorageBackendConfigurationAction(parent, rows, columns, elementSize, configuration, initializeFromConfiguration, currentElementTypeIndex),
    _rows(rows),
    _columns(columns),
    _sourceElementTypeIndex(currentElementTypeIndex),
    _hasTiledSource(initializeFromConfiguration),
    _memoryObservationAction(this)
{
    enableConversionPerformanceOptions(configuration.conversionWorkerCount, configuration.conversionChunkTileRows);

    if (!materializesFullMatrix)
    {
        moveElementTypeAnalysisActionToEnd();
        return;
    }

    _checksMaterialization = true;
    addAction(&_memoryObservationAction);

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
        _memoryObservationAction.setMessage(util::SeverityLevel::Error, "Memory", "The tiled matrix size exceeds the addressable memory limit, so this conversion cannot run.");
        return;
    }

    const auto denseBytes = elementCount * *targetElementSize;
    auto estimatedBytes = denseBytes;
    const auto configuration = getConfiguration();
    const auto resultEstimate = pointdata::tiled::PointDataTiledStorage::estimateMemory(
        _rows,
        _columns,
        *targetElementSize,
        configuration.rowTileSize,
        configuration.columnTileSize,
        configuration.cacheCapacity,
        configuration.cacheCapacityBytes);
    const auto packedRetile = _hasTiledSource && configuration.packedTiles && getTargetElementTypeIndex() == _sourceElementTypeIndex;
    const auto resultResidentBytes = packedRetile
        ? resultEstimate.indexBytes + resultEstimate.cachedTileBytes
        : resultEstimate.indexBytes + resultEstimate.rawDataBytes;
    if (_hasTiledSource && configuration.packedTiles && getTargetElementTypeIndex() == _sourceElementTypeIndex)
    {
        const auto tileRows = std::min<std::uint64_t>(_rows, configuration.rowTileSize);
        const auto tileColumns = std::min<std::uint64_t>(_columns, configuration.columnTileSize);
        if (tileRows != 0 && tileColumns > std::numeric_limits<std::uint64_t>::max() / tileRows)
        {
            _canApply = false;
            _memoryObservationAction.setMessage(util::SeverityLevel::Error, "Memory", "The packed tile working-set estimate exceeds the addressable memory limit, so this conversion cannot run.");
            return;
        }

        const auto tileElements = tileRows * tileColumns;
        if (tileElements != 0 && *targetElementSize > std::numeric_limits<std::uint64_t>::max() / tileElements)
        {
            _canApply = false;
            _memoryObservationAction.setMessage(util::SeverityLevel::Error, "Memory", "The packed tile working-set estimate exceeds the addressable memory limit, so this conversion cannot run.");
            return;
        }

        const auto tileBytes = tileElements * *targetElementSize;
        if (tileBytes > std::numeric_limits<std::uint64_t>::max() / 2)
        {
            _canApply = false;
            _memoryObservationAction.setMessage(util::SeverityLevel::Error, "Memory", "The packed tile working-set estimate exceeds the addressable memory limit, so this conversion cannot run.");
            return;
        }

        const auto tileWorkingBytes = tileBytes * 2;
        const auto workerCount = std::clamp<std::size_t>(configuration.conversionWorkerCount, 1, 32);
        if (tileWorkingBytes != 0 && workerCount > std::numeric_limits<std::uint64_t>::max() / tileWorkingBytes)
        {
            _canApply = false;
            _memoryObservationAction.setMessage(util::SeverityLevel::Error, "Memory", "The packed tile working-set estimate exceeds the addressable memory limit, so this conversion cannot run.");
            return;
        }

        estimatedBytes = tileWorkingBytes * workerCount;
    }
    const auto availableRamBytes = getAvailablePhysicalMemoryBytes();
    if (!availableRamBytes)
    {
        _memoryObservationAction.setMessage(util::SeverityLevel::Warning, "Memory", QString("Result payload is %1 uncompressed; estimated resident result footprint is %2. Conversion working memory is estimated at %3, but currently available physical memory could not be determined.")
            .arg(util::getNoBytesHumanReadable(denseBytes), util::getNoBytesHumanReadable(resultResidentBytes), util::getNoBytesHumanReadable(estimatedBytes)));
        return;
    }

    _canApply = estimatedBytes <= *availableRamBytes;
    if (!_canApply)
        _memoryObservationAction.setMessage(util::SeverityLevel::Error, "Memory", QString("Result payload is %1 uncompressed; estimated resident result footprint is %2. Conversion working memory is estimated at %3, above the %4 of currently available physical memory. Conversion is disabled; close other applications or choose a smaller tile size or worker count.")
            .arg(util::getNoBytesHumanReadable(denseBytes), util::getNoBytesHumanReadable(resultResidentBytes), util::getNoBytesHumanReadable(estimatedBytes), util::getNoBytesHumanReadable(*availableRamBytes)));
    else
    {
        const auto severity = estimatedBytes > *availableRamBytes - *availableRamBytes / 4 ? util::SeverityLevel::Warning : util::SeverityLevel::Info;
        const auto description = _hasTiledSource && configuration.packedTiles ? QStringLiteral("Chunked packed conversion working memory") : QStringLiteral("Tiled materialization");
        _memoryObservationAction.setMessage(severity, "Memory", QString("Result payload is %1 uncompressed; estimated resident result footprint is %2. %3 is estimated at %4; currently available physical memory is %5. Memory use can change before conversion starts, so the estimate is only a preflight check.")
            .arg(util::getNoBytesHumanReadable(denseBytes), util::getNoBytesHumanReadable(resultResidentBytes), description, util::getNoBytesHumanReadable(estimatedBytes), util::getNoBytesHumanReadable(*availableRamBytes)));
    }
}

bool TiledStorageBackendConversionAction::canApply() const
{
    return _canApply;
}

}
