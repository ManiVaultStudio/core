// SPDX-License-Identifier: LGPL-3.0-or-later

#include "TiledStorageBackendConversionAction.h"

#include "../interface/StorageBackendMemory.h"

#include <util/Miscellaneous.h>

#include <limits>

namespace mv::gui
{

TiledStorageBackendConversionAction::TiledStorageBackendConversionAction(QObject* parent, const std::uint64_t rows, const std::uint64_t columns, const std::size_t elementSize, const pointdata::storage::StorageBackendConfiguration& configuration, const bool initializeFromConfiguration, const bool materializesFullMatrix, const std::uint32_t currentElementTypeIndex) :
    TiledStorageBackendConfigurationAction(parent, rows, columns, elementSize, configuration, initializeFromConfiguration, currentElementTypeIndex),
    _rows(rows),
    _columns(columns),
    _memoryObservationAction(this)
{
    if (!materializesFullMatrix)
        return;

    _checksMaterialization = true;
    addAction(&_memoryObservationAction);

    connect(this, &StorageBackendConversionAction::optionsChanged, this, &TiledStorageBackendConversionAction::updateMemoryObservation);
    updateMemoryObservation();
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
    const auto availableRamBytes = getAvailablePhysicalMemoryBytes();
    if (!availableRamBytes)
    {
        _memoryObservationAction.setMessage(util::SeverityLevel::Warning, "Memory", QString("Tiled materialization is estimated at %1, but currently available physical memory could not be determined. The conversion may fail if enough memory is unavailable.").arg(util::getNoBytesHumanReadable(denseBytes)));
        return;
    }

    _canApply = denseBytes <= *availableRamBytes;
    if (!_canApply)
        _memoryObservationAction.setMessage(util::SeverityLevel::Error, "Memory", QString("Tiled materialization is estimated at %1, above the %2 of currently available physical memory. Conversion is disabled; close other applications or choose a sparse representation.").arg(util::getNoBytesHumanReadable(denseBytes), util::getNoBytesHumanReadable(*availableRamBytes)));
    else
    {
        const auto severity = denseBytes > *availableRamBytes - *availableRamBytes / 4 ? util::SeverityLevel::Warning : util::SeverityLevel::Info;
        _memoryObservationAction.setMessage(severity, "Memory", QString("Tiled materialization is estimated at %1; currently available physical memory is %2. Memory use can change before conversion starts, so the estimate is only a preflight check.").arg(util::getNoBytesHumanReadable(denseBytes), util::getNoBytesHumanReadable(*availableRamBytes)));
    }
}

bool TiledStorageBackendConversionAction::canApply() const
{
    return _canApply;
}

}
