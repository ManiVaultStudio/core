// SPDX-License-Identifier: LGPL-3.0-or-later

#include "SparseStorageBackendConversionAction.h"

#include "../interface/PointsStorageBackendTypes.h"
#include "../interface/StorageBackendMemory.h"

#include <util/Miscellaneous.h>

#include <limits>

namespace mv::gui
{

SparseStorageBackendConversionAction::SparseStorageBackendConversionAction(QObject* parent, const std::uint64_t rows, const std::uint64_t columns, const std::uint32_t currentElementTypeIndex, const bool columnCompressed) :
    StorageBackendConversionAction(parent, "Sparse storage", currentElementTypeIndex),
    _rows(rows),
    _columns(columns),
    _layoutAction(this, "Layout", { "Row-compressed (CSR)", "Column-compressed (CSC)" }, columnCompressed ? "Column-compressed (CSC)" : "Row-compressed (CSR)"),
    _memoryObservationAction(this)
{
    setShowLabels(true);
    addAction(&_layoutAction);
    addAction(&_memoryObservationAction);

    connect(&_layoutAction, &OptionAction::currentIndexChanged, this, [this] {
        updateObservation();
        notifyOptionsChanged();
    });
    connect(this, &StorageBackendConversionAction::optionsChanged, this, &SparseStorageBackendConversionAction::updateObservation);

    updateObservation();
}

pointdata::storage::StorageBackendType SparseStorageBackendConversionAction::getTargetBackendType() const
{
    return pointdata::storage::StorageBackendType::Sparse;
}

pointdata::storage::StorageBackendConfigurationOptionsPtr SparseStorageBackendConversionAction::getConversionOptions() const
{
    auto configuration = std::make_shared<pointdata::storage::SparseStorageBackendConfiguration>();
    configuration->columnCompressed = _layoutAction.getCurrentIndex() == 1;
    configuration->targetElementTypeIndex = getTargetElementTypeIndex();
    return configuration;
}

bool SparseStorageBackendConversionAction::canApply() const
{
    const auto compressedSize = _layoutAction.getCurrentIndex() == 1 ? _columns : _rows;
    const auto elementSize = getTargetElementSize();
    return elementSize && *elementSize != 0 && _columns <= std::numeric_limits<std::size_t>::max() / *elementSize &&
        _rows < std::numeric_limits<std::size_t>::max() && compressedSize < std::numeric_limits<std::size_t>::max() &&
        !(_rows != 0 && _columns > std::numeric_limits<std::uint64_t>::max() / _rows);
}

void SparseStorageBackendConversionAction::updateObservation()
{
    const auto compressedSize = _layoutAction.getCurrentIndex() == 1 ? _columns : _rows;
    const auto elementSize = getTargetElementSize();
    if (!elementSize || *elementSize == 0 || _columns > std::numeric_limits<std::size_t>::max() / *elementSize || _rows >= std::numeric_limits<std::size_t>::max() || compressedSize >= std::numeric_limits<std::size_t>::max())
    {
        _memoryObservationAction.setMessage(util::SeverityLevel::Error, "Memory", "The selected sparse conversion requires a row or pointer array larger than this process can address.");
        return;
    }

    if (_rows != 0 && _columns > std::numeric_limits<std::uint64_t>::max() / _rows)
    {
        _memoryObservationAction.setMessage(util::SeverityLevel::Error, "Memory", "The matrix dimensions overflow the supported element count.");
        return;
    }

    const auto elementCount = _rows * _columns;
    const auto indexSize = elementCount > std::numeric_limits<std::uint64_t>::max() / sizeof(std::uint64_t) ? std::numeric_limits<std::uint64_t>::max() : elementCount * sizeof(std::uint64_t);
    const auto valueSize = elementCount > std::numeric_limits<std::uint64_t>::max() / *elementSize ? std::numeric_limits<std::uint64_t>::max() : elementCount * *elementSize;
    const auto pointerCount = compressedSize + 1;
    const auto pointerSize = pointerCount > std::numeric_limits<std::uint64_t>::max() / sizeof(std::uint64_t) ? std::numeric_limits<std::uint64_t>::max() : pointerCount * sizeof(std::uint64_t);
    const auto worstCaseBytes = indexSize > std::numeric_limits<std::uint64_t>::max() - valueSize || indexSize + valueSize > std::numeric_limits<std::uint64_t>::max() - pointerSize ? std::numeric_limits<std::uint64_t>::max() : indexSize + valueSize + pointerSize;
    const auto availableRamBytes = getAvailablePhysicalMemoryBytes();

    auto message = QString("The conversion scans all %1 matrix values. In the worst case, if none are zero, the compressed arrays may use up to %2; actual use depends on sparsity.")
        .arg(util::getIntegerCountHumanReadable(static_cast<double>(elementCount)))
        .arg(util::getNoBytesHumanReadable(worstCaseBytes));

    if (_layoutAction.getCurrentIndex() == 1)
        message += " Column-compressed conversion also holds temporary index and value buffers while arranging the CSC layout.";

    const auto severity = availableRamBytes && worstCaseBytes > *availableRamBytes ? util::SeverityLevel::Warning : util::SeverityLevel::Info;
    if (availableRamBytes)
        message += QString(" Currently available physical memory is %1; because this is a worst-case estimate, the actual conversion may need much less.").arg(util::getNoBytesHumanReadable(*availableRamBytes));
    else
        message += " Currently available physical memory could not be determined.";

    _memoryObservationAction.setMessage(severity, "Memory", message);
}

}
