// SPDX-License-Identifier: LGPL-3.0-or-later

#include "SparseStorageBackendConversionAction.h"

#include "../interface/PointsStorageBackendTypes.h"
#include "../interface/StorageBackendMemory.h"

#include <util/Miscellaneous.h>

#include <limits>
#include <algorithm>

namespace mv::gui
{

SparseStorageBackendConversionAction::SparseStorageBackendConversionAction(QObject* parent, const std::uint64_t rows, const std::uint64_t columns, const std::uint32_t currentElementTypeIndex, const bool columnCompressed, const std::size_t workerCount, const std::size_t chunkRows) :
    StorageBackendConversionAction(parent, "Sparse storage", currentElementTypeIndex),
    _rows(rows),
    _columns(columns),
    _initialColumnCompressed(columnCompressed),
    _layoutAction(this, "Layout", { "Row-compressed (CSR)", "Column-compressed (CSC)" }, columnCompressed ? "Column-compressed (CSC)" : "Row-compressed (CSR)"),
    _memoryObservationAction(this),
    _performanceAction(this, "Conversion performance"),
    _performanceSummaryAction(this, ""),
    _performanceSettingsAction(this, "Settings"),
    _workerCountAction(this, "Worker count", 1, 32, static_cast<std::int32_t>(std::clamp<std::size_t>(workerCount, 1, 32))),
    _chunkRowsAction(this, "Rows per chunk", 1, 65536, static_cast<std::int32_t>(std::clamp<std::size_t>(chunkRows, 1, 65536))),
    _initialWorkerCount(std::clamp<std::size_t>(workerCount, 1, 32)),
    _initialChunkRows(std::clamp<std::size_t>(chunkRows, 1, 65536))
{
    setShowLabels(true);
    addAction(&_layoutAction);
    _performanceAction.setShowLabels(false);
    _performanceAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _performanceAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ToolButtonAutoRaise);
    _performanceAction.setIconByName("sliders");
    _performanceSummaryAction.setDefaultWidgetFlags(StringAction::LineEdit);
    _performanceSummaryAction.setEnabled(false);
    _performanceSummaryAction.setStretch(1);
    _performanceSettingsAction.setShowLabels(true);
    _performanceSettingsAction.setLabelSizingType(LabelSizingType::Auto);
    _performanceSettingsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _performanceSettingsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ToolButtonAutoRaise);
    _performanceSettingsAction.setIconByName("sliders");
    _workerCountAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _chunkRowsAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _workerCountAction.setToolTip("Maximum worker threads used to scan source rows.");
    _chunkRowsAction.setToolTip("Number of rows assigned to one bounded conversion chunk.");
    _performanceAction.addAction(&_performanceSummaryAction);
    _performanceAction.addAction(&_performanceSettingsAction);
    _performanceSettingsAction.addAction(&_workerCountAction);
    _performanceSettingsAction.addAction(&_chunkRowsAction);
    addAction(&_performanceAction);
    addAction(&_memoryObservationAction);

    connect(&_layoutAction, &OptionAction::currentIndexChanged, this, [this] {
        updateObservation();
        notifyOptionsChanged();
    });
    const auto updatePerformance = [this] {
        _performanceSummaryAction.setString(QStringLiteral("%1 workers, %2 rows per chunk")
            .arg(_workerCountAction.getValue())
            .arg(_chunkRowsAction.getValue()));
        updateObservation();
        notifyOptionsChanged();
    };
    connect(&_workerCountAction, &IntegralAction::valueChanged, this, updatePerformance);
    connect(&_chunkRowsAction, &IntegralAction::valueChanged, this, updatePerformance);
    connect(this, &StorageBackendConversionAction::optionsChanged, this, &SparseStorageBackendConversionAction::updateObservation);

    updatePerformance();
    updateObservation();
    moveElementTypeAnalysisActionToEnd();
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
    configuration->conversionWorkerCount = static_cast<std::size_t>(_workerCountAction.getValue());
    configuration->conversionChunkRows = static_cast<std::size_t>(_chunkRowsAction.getValue());
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

bool SparseStorageBackendConversionAction::hasChanges() const
{
    return StorageBackendConversionAction::hasChanges() || (_layoutAction.getCurrentIndex() == 1) != _initialColumnCompressed ||
        static_cast<std::size_t>(_workerCountAction.getValue()) != _initialWorkerCount || static_cast<std::size_t>(_chunkRowsAction.getValue()) != _initialChunkRows;
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
    const auto configuredRowsPerChunk = static_cast<std::uint64_t>(std::max(1, _chunkRowsAction.getValue()));
    const auto chunkCount = _rows / configuredRowsPerChunk + (_rows % configuredRowsPerChunk != 0);
    const auto workers = std::min<std::uint64_t>(static_cast<std::uint64_t>(_workerCountAction.getValue()), std::max<std::uint64_t>(1, chunkCount));
    const auto rowsPerChunk = std::min<std::uint64_t>(_rows, configuredRowsPerChunk);
    const auto saturatedMultiply = [](const std::uint64_t left, const std::uint64_t right) {
        return left != 0 && right > std::numeric_limits<std::uint64_t>::max() / left ? std::numeric_limits<std::uint64_t>::max() : left * right;
    };
    const auto saturatedAdd = [](const std::uint64_t left, const std::uint64_t right) {
        return left > std::numeric_limits<std::uint64_t>::max() - right ? std::numeric_limits<std::uint64_t>::max() : left + right;
    };
    // Chunk-local sparse arrays coexist with the flattened output during assembly.
    // CSC additionally retains CSR while the column-compressed arrays are built.
    const auto sparseArrayCopies = _layoutAction.getCurrentIndex() == 1 ? 3u : 2u;
    const auto chunkOutputBytes = saturatedMultiply(worstCaseBytes, sparseArrayCopies);
    const auto workerRowBytes = saturatedMultiply(saturatedMultiply(_columns, *elementSize), workers);
    const auto estimatedWorkingBytes = saturatedAdd(chunkOutputBytes, workerRowBytes);
    const auto availableRamBytes = getAvailablePhysicalMemoryBytes();

    auto message = QString("Estimated sparse result footprint: up to %2 if every one of the %1 matrix values is nonzero; the actual total depends on sparsity. It includes the pointer, index, and value arrays.")
        .arg(util::getIntegerCountHumanReadable(static_cast<double>(elementCount)))
        .arg(util::getNoBytesHumanReadable(worstCaseBytes));
    message += QString(" Conversion working memory, including chunk assembly and one source row buffer for each of up to %1 workers (%2 rows per chunk), is estimated at %3.")
        .arg(workers)
        .arg(rowsPerChunk)
        .arg(util::getNoBytesHumanReadable(estimatedWorkingBytes));

    if (_layoutAction.getCurrentIndex() == 1)
        message += " Column-compressed conversion also holds temporary index and value buffers while arranging the CSC layout.";

    const auto severity = availableRamBytes && estimatedWorkingBytes > *availableRamBytes ? util::SeverityLevel::Warning : util::SeverityLevel::Info;
    if (availableRamBytes)
        message += QString(" Currently available physical memory is %1; because this is a worst-case estimate, the actual conversion may need much less.").arg(util::getNoBytesHumanReadable(*availableRamBytes));
    else
        message += " Currently available physical memory could not be determined.";

    _memoryObservationAction.setMessage(severity, "Memory", message);
}

}
