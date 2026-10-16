// SPDX-License-Identifier: LGPL-3.0-or-later

#include "TiledStorageBackendConversionAction.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace mv::gui
{

namespace
{

constexpr std::uint64_t targetTileElements = 65536;

std::size_t clampTileSize(const std::uint64_t extent, const std::uint64_t value)
{
    return static_cast<std::size_t>(std::min(extent, std::max<std::uint64_t>(1, value)));
}

std::size_t balancedTileSize(const std::uint64_t extent, const std::uint64_t otherExtent)
{
    if (extent == 0 || otherExtent == 0)
        return 1;

    const auto ideal = std::sqrt(static_cast<double>(targetTileElements) * static_cast<double>(extent) / static_cast<double>(otherExtent));
    return clampTileSize(extent, static_cast<std::uint64_t>(ideal));
}

std::size_t cacheCapacity(const std::uint64_t rows, const std::uint64_t columns, const std::size_t rowTileSize, const std::size_t columnTileSize)
{
    const auto rowTileCount    = (rows + rowTileSize - 1) / rowTileSize;
    const auto columnTileCount = (columns + columnTileSize - 1) / columnTileSize;
    const auto tileCount       = rowTileCount * columnTileCount;
    const auto capacity        = static_cast<std::uint64_t>(std::sqrt(static_cast<double>(std::max<std::uint64_t>(1, tileCount))));

    return static_cast<std::size_t>(std::clamp<std::uint64_t>(capacity, 1, 64));
}

}

TiledStorageBackendConversionAction::TiledStorageBackendConversionAction(QObject* parent, const std::uint64_t rows, const std::uint64_t columns, const pointdata::storage::StorageBackendConfiguration& configuration, const bool initializeFromConfiguration) :
    StorageBackendConversionAction(parent, "Tiled storage"),
    _rows(rows),
    _columns(columns),
    _presetAction(this, "Tile preset", { "Default", "Row dominant", "Column dominant", "Custom" }, "Default"),
    _rowTileSizeAction(this, "Rows per tile", 1, std::numeric_limits<std::int32_t>::max(), 1),
    _columnTileSizeAction(this, "Columns per tile", 1, std::numeric_limits<std::int32_t>::max(), 1),
    _cacheCapacityAction(this, "Tile cache capacity", 1, std::numeric_limits<std::int32_t>::max(), 1)
{
    setShowLabels(true);

    _presetAction.setDefaultWidgetFlags(OptionAction::HorizontalButtons);
    _rowTileSizeAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _columnTileSizeAction.setDefaultWidgetFlags(IntegralAction::SpinBox);
    _cacheCapacityAction.setDefaultWidgetFlags(IntegralAction::SpinBox);

    addAction(&_presetAction);
    addAction(&_rowTileSizeAction);
    addAction(&_columnTileSizeAction);
    addAction(&_cacheCapacityAction);

    connect(&_presetAction, &OptionAction::currentIndexChanged, this, [this](const std::int32_t presetIndex) {
        applyPreset(presetIndex);
        notifyOptionsChanged();
    });

    connect(&_rowTileSizeAction, &IntegralAction::valueChanged, this, [this] {
        notifyOptionsChanged();
    });

    connect(&_columnTileSizeAction, &IntegralAction::valueChanged, this, [this] {
        notifyOptionsChanged();
    });

    connect(&_cacheCapacityAction, &IntegralAction::valueChanged, this, [this] {
        notifyOptionsChanged();
    });

    applyPreset(0);

    if (initializeFromConfiguration)
    {
        _presetAction.setCurrentText("Custom");
        _rowTileSizeAction.setValue(static_cast<std::int32_t>(configuration.rowTileSize));
        _columnTileSizeAction.setValue(static_cast<std::int32_t>(configuration.columnTileSize));
        _cacheCapacityAction.setValue(static_cast<std::int32_t>(configuration.cacheCapacity));
    }
}

pointdata::storage::StorageBackendType TiledStorageBackendConversionAction::getTargetBackendType() const
{
    return pointdata::storage::StorageBackendType::Tiled;
}

pointdata::storage::StorageBackendConversionOptionsPtr TiledStorageBackendConversionAction::getConversionOptions() const
{
    auto configuration = std::make_shared<pointdata::storage::StorageBackendConfiguration>();
    configuration->rowTileSize    = static_cast<std::size_t>(_rowTileSizeAction.getValue());
    configuration->columnTileSize = static_cast<std::size_t>(_columnTileSizeAction.getValue());
    configuration->cacheCapacity  = static_cast<std::size_t>(_cacheCapacityAction.getValue());
    return configuration;
}

void TiledStorageBackendConversionAction::applyPreset(const std::int32_t presetIndex)
{
    if (presetIndex == 3)
        return;

    std::size_t rowTileSize    = balancedTileSize(_rows, _columns);
    std::size_t columnTileSize = balancedTileSize(_columns, _rows);

    if (presetIndex == 1)
    {
        columnTileSize = clampTileSize(_columns, std::min<std::uint64_t>(_columns, 256));
        rowTileSize    = clampTileSize(_rows, targetTileElements / std::max<std::size_t>(1, columnTileSize));
    }
    else if (presetIndex == 2)
    {
        rowTileSize    = clampTileSize(_rows, std::min<std::uint64_t>(_rows, 256));
        columnTileSize = clampTileSize(_columns, targetTileElements / std::max<std::size_t>(1, rowTileSize));
    }

    _rowTileSizeAction.setValue(static_cast<std::int32_t>(rowTileSize));
    _columnTileSizeAction.setValue(static_cast<std::int32_t>(columnTileSize));
    _cacheCapacityAction.setValue(static_cast<std::int32_t>(cacheCapacity(_rows, _columns, rowTileSize, columnTileSize)));
}

}
