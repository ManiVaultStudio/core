// SPDX-License-Identifier: LGPL-3.0-or-later

#include "SparseStorageBackendConversionAction.h"

#include "../interface/PointsStorageBackendTypes.h"

#include <Application.h>
#include <CoreInterface.h>

#include <limits>
#include <algorithm>

namespace mv::gui
{

SparseStorageBackendConversionAction::SparseStorageBackendConversionAction(QObject* parent, const std::uint64_t rows, const std::uint64_t columns, const std::uint32_t currentElementTypeIndex, const bool columnCompressed, const std::size_t workerCount, const std::size_t chunkRows, const bool storeRowCompressed, const bool storeColumnCompressed) :
    StorageBackendConversionAction(parent, "Sparse storage", currentElementTypeIndex),
    _rows(rows),
    _columns(columns),
    _initialColumnCompressed(columnCompressed),
    _initialStoreRowCompressed(storeRowCompressed && !columnCompressed),
    _initialStoreColumnCompressed(storeColumnCompressed || columnCompressed),
    _storeRowsAction(this, "Store CSR index for row access", storeRowCompressed && !columnCompressed),
    _storeColumnsAction(this, "Store CSC index for column access", storeColumnCompressed || columnCompressed),
    _workerCountAction(this, "Worker count", 1, 32, static_cast<std::int32_t>(std::clamp<std::size_t>(workerCount, 1, 32))),
    _chunkRowsAction(this, "Rows per chunk", 1, 65536, static_cast<std::int32_t>(std::clamp<std::size_t>(chunkRows, 1, 65536))),
    _initialWorkerCount(std::clamp<std::size_t>(workerCount, 1, 32)),
    _initialChunkRows(std::clamp<std::size_t>(chunkRows, 1, 65536))
{
    _storeRowsAction.setDefaultWidgetFlags(ToggleAction::CheckBox);
    _storeRowsAction.setDescription("<p>Store rows contiguously for faster row reads. This is the traditional CSR representation.</p>");
    _storeColumnsAction.setDefaultWidgetFlags(ToggleAction::CheckBox);
    _storeColumnsAction.setDescription("<p>Store columns contiguously for faster column reads. When both indexes are enabled, values are duplicated, increasing disk and memory use.</p>");
    addAction(&_storeRowsAction);
    addAction(&_storeColumnsAction);
    _workerCountAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _chunkRowsAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _workerCountAction.setToolTip("Maximum worker threads used to build the sparse indexes.");
    _chunkRowsAction.setToolTip("Number of source rows assigned to one bounded conversion chunk.");
    _workerCountAction.setDescription("<p>Maximum worker threads used to scan source rows and build sparse indexes. Higher values may improve conversion speed at the cost of additional CPU and memory use.</p>");
    _chunkRowsAction.setDescription("<p>Number of source rows processed in one conversion chunk. Larger chunks can improve throughput but use more temporary memory.</p>");
    addAdvancedAction(&_workerCountAction);
    addAdvancedAction(&_chunkRowsAction);
    auto& performanceGroup = getAdvancedOptionsGroupAction();
    performanceGroup.setText(tr("Performance"));
    performanceGroup.setIconByName("code");
    performanceGroup.setSubText(tr("Developer-only"));
    performanceGroup.setDescription("<p>Controls the worker limit and row chunk size used while building the sparse indexes. More workers may improve conversion speed while using more CPU and memory.</p>");
    auto& developerModeAction = mv::settings().getDeveloperSettingsAction().getEnableDeveloperAddonsAction();
    const auto updatePerformanceVisibility = [this, &developerModeAction] {
        setGroupActionVisibility(&getAdvancedOptionsGroupAction(), developerModeAction.isChecked());
    };
    connect(&developerModeAction, &ToggleAction::toggled, this, updatePerformanceVisibility);
    updatePerformanceVisibility();
    const auto updateIndexes = [this] { notifyOptionsChanged(); };
    connect(&_storeRowsAction, &ToggleAction::toggled, this, updateIndexes);
    connect(&_storeColumnsAction, &ToggleAction::toggled, this, updateIndexes);
    const auto updatePerformance = [this] { notifyOptionsChanged(); };
    connect(&_workerCountAction, &IntegralAction::valueChanged, this, updatePerformance);
    connect(&_chunkRowsAction, &IntegralAction::valueChanged, this, updatePerformance);
    moveElementTypeAnalysisActionToEnd();
}

pointdata::storage::StorageBackendType SparseStorageBackendConversionAction::getTargetBackendType() const
{
    return pointdata::storage::StorageBackendType::Sparse;
}

pointdata::storage::StorageBackendConfigurationOptionsPtr SparseStorageBackendConversionAction::getConversionOptions() const
{
    auto configuration = std::make_shared<pointdata::storage::SparseStorageBackendConfiguration>();
    configuration->storeRowCompressed = _storeRowsAction.isChecked();
    configuration->storeColumnCompressed = _storeColumnsAction.isChecked();
    configuration->columnCompressed = !configuration->storeRowCompressed;
    configuration->targetElementTypeIndex = getTargetElementTypeIndex();
    configuration->conversionWorkerCount = static_cast<std::size_t>(_workerCountAction.getValue());
    configuration->conversionChunkRows = static_cast<std::size_t>(_chunkRowsAction.getValue());
    return configuration;
}

bool SparseStorageBackendConversionAction::canApply() const
{
    const auto elementSize = getTargetElementSize();
    return (_storeRowsAction.isChecked() || _storeColumnsAction.isChecked()) && elementSize && *elementSize != 0 && _columns <= std::numeric_limits<std::size_t>::max() / *elementSize &&
        _rows < std::numeric_limits<std::size_t>::max() && _columns < std::numeric_limits<std::size_t>::max() &&
        !(_rows != 0 && _columns > std::numeric_limits<std::uint64_t>::max() / _rows);
}

bool SparseStorageBackendConversionAction::hasChanges() const
{
    const auto storesRows = _storeRowsAction.isChecked();
    const auto storesColumns = _storeColumnsAction.isChecked();
    return StorageBackendConversionAction::hasChanges() || storesRows != _initialStoreRowCompressed || storesColumns != _initialStoreColumnCompressed ||
        (!(_initialStoreRowCompressed || _initialStoreColumnCompressed) && storesColumns != _initialColumnCompressed) ||
        static_cast<std::size_t>(_workerCountAction.getValue()) != _initialWorkerCount || static_cast<std::size_t>(_chunkRowsAction.getValue()) != _initialChunkRows;
}

}
