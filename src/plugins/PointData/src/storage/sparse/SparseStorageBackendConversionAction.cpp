// SPDX-License-Identifier: LGPL-3.0-or-later

#include "SparseStorageBackendConversionAction.h"

#include "../interface/PointsStorageBackendTypes.h"

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
    _performanceAction(this, "Conversion performance"),
    _performanceSummaryAction(this, ""),
    _performanceSettingsAction(this, "Settings"),
    _workerCountAction(this, "Worker count", 1, 32, static_cast<std::int32_t>(std::clamp<std::size_t>(workerCount, 1, 32))),
    _chunkRowsAction(this, "Rows per chunk", 1, 65536, static_cast<std::int32_t>(std::clamp<std::size_t>(chunkRows, 1, 65536))),
    _initialWorkerCount(std::clamp<std::size_t>(workerCount, 1, 32)),
    _initialChunkRows(std::clamp<std::size_t>(chunkRows, 1, 65536))
{
    _layoutAction.setDescription(
        "<p>Choose the compressed orientation for sparse data: CSR stores rows contiguously and generally suits row access, while CSC stores columns contiguously and generally suits column access. This changes the storage layout, not the logical matrix values.</p>");
    addAction(&_layoutAction);
    _performanceAction.setShowLabels(false);
    _performanceAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _performanceAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ToolButtonAutoRaise);
    _performanceAction.setIconByName("sliders");
    _performanceSummaryAction.setDefaultWidgetFlags(StringAction::LineEdit);
    _performanceSummaryAction.setEnabled(false);
    _performanceSummaryAction.setStretch(1);
    _performanceSettingsAction.setShowLabels(true);
    _performanceSettingsAction.setLabelSizingType(GroupAction::LabelSizingType::Fixed);
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
    addAdvancedAction(&_performanceAction);
    connect(&_layoutAction, &OptionAction::currentIndexChanged, this, [this] {
        notifyOptionsChanged();
    });
    const auto updatePerformance = [this] {
        _performanceSummaryAction.setString(QStringLiteral("%1 workers, %2 rows per chunk")
            .arg(_workerCountAction.getValue())
            .arg(_chunkRowsAction.getValue()));
        notifyOptionsChanged();
    };
    connect(&_workerCountAction, &IntegralAction::valueChanged, this, updatePerformance);
    connect(&_chunkRowsAction, &IntegralAction::valueChanged, this, updatePerformance);
    updatePerformance();
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

}
