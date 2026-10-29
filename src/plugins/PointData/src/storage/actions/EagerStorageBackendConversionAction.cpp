// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EagerStorageBackendConversionAction.h"

#include "StorageBackendMemory.h"

#include <util/Miscellaneous.h>

#include <limits>

namespace mv::gui
{

EagerStorageBackendConversionAction::EagerStorageBackendConversionAction(QObject* parent, const std::uint64_t rows, const std::uint64_t columns, const std::size_t elementSize) :
    StorageBackendConversionAction(parent, "In-memory storage"),
    _rows(rows),
    _columns(columns),
    _elementSize(elementSize),
    _memoryObservationAction(this)
{
    setShowLabels(true);
    addAction(&_memoryObservationAction);

    const auto invalidDimensions = _rows != 0 && _columns > (std::numeric_limits<std::uint64_t>::max)() / _rows;
    const auto elementCount = invalidDimensions ? 0 : _rows * _columns;
    _canApply = !invalidDimensions && _elementSize != 0 && elementCount <= (std::numeric_limits<std::size_t>::max)() / _elementSize;

    if (!_canApply)
    {
        _memoryObservationAction.setMessage(util::SeverityLevel::Error, "Memory", "The dense matrix size exceeds the addressable memory limit, so this conversion cannot run.");
        return;
    }

    const auto denseBytes = elementCount * _elementSize;
    const auto availableRamBytes = getAvailablePhysicalMemoryBytes();
    const auto exceedsAvailableRam = availableRamBytes && denseBytes > *availableRamBytes;
    _canApply = !exceedsAvailableRam;

    if (exceedsAvailableRam)
        _memoryObservationAction.setMessage(util::SeverityLevel::Error, "Memory", QString("Dense materialization is estimated at %1, above the %2 of currently available physical memory. Conversion is disabled; close other applications or choose a tiled representation.")
            .arg(util::getNoBytesHumanReadable(denseBytes), util::getNoBytesHumanReadable(*availableRamBytes)));
    else if (availableRamBytes)
    {
        const auto severity = denseBytes > *availableRamBytes - *availableRamBytes / 4 ? util::SeverityLevel::Warning : util::SeverityLevel::Info;
        _memoryObservationAction.setMessage(severity, "Memory", QString("Dense materialization is estimated at %1; currently available physical memory is %2. Memory use can change before conversion starts, so the estimate is only a preflight check.")
            .arg(util::getNoBytesHumanReadable(denseBytes), util::getNoBytesHumanReadable(*availableRamBytes)));
    }
    else
        _memoryObservationAction.setMessage(util::SeverityLevel::Warning, "Memory", QString("Dense materialization is estimated at %1, but currently available physical memory could not be determined. The conversion may fail if enough memory is unavailable.").arg(util::getNoBytesHumanReadable(denseBytes)));
}

pointdata::storage::StorageBackendType EagerStorageBackendConversionAction::getTargetBackendType() const
{
    return pointdata::storage::StorageBackendType::Eager;
}

pointdata::storage::StorageBackendConfigurationOptionsPtr EagerStorageBackendConversionAction::getConversionOptions() const
{
    return {};
}

bool EagerStorageBackendConversionAction::canApply() const
{
    return _canApply;
}

}
