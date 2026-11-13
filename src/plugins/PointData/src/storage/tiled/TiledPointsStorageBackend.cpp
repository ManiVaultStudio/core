// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "TiledPointsStorageBackend.h"

#include "../dense/DenseStorageBackendConversionAction.h"
#include "../interface/StorageBackendConversionAction.h"
#include "../sparse/SparseStorageBackendConversionAction.h"
#include "TiledStorageBackendConversionAction.h"
#include "PointDataTiledSerialization.h"

#include <algorithm>
#include <cstring>
#include <limits>
#include <stdexcept>

#include <workflow/WorkflowPlan.h>
#include <workflow/WorkflowExecutionContext.h>

namespace mv::pointdata::storage
{

namespace
{

PointDataAccessView makeTileAccessView(const mv::pointdata::tiled::TileView& tile, const std::uint64_t rowOffset, const std::uint64_t columnOffset, const std::uint64_t rows, const std::uint64_t columns, const std::uint32_t typeIndex)
{
    PointDataAccessView view;
    view.data = static_cast<const char*>(tile.data())
        + rowOffset * tile.columns() * tile.elementSize()
        + columnOffset * tile.elementSize();
    view.rows             = rows;
    view.columns          = columns;
    view.rowStrideBytes   = tile.columns() * tile.elementSize();
    view.elementTypeIndex = typeIndex;
    view.elementSizeBytes = static_cast<std::uint32_t>(tile.elementSize());
    view.lifetime         = tile.lifetimeToken();
    return view;
}

}

TiledPointsStorageBackend::TiledPointsStorageBackend(const std::shared_ptr<mv::pointdata::tiled::PointDataTiledStorage>& storage, const std::uint32_t elementTypeIndex) :
    _storage(storage),
    _elementTypeIndex(elementTypeIndex)
{
}

std::shared_ptr<mv::pointdata::tiled::PointDataTiledStorage> TiledPointsStorageBackend::create(const TiledStorageBackendCreationOptions& options, StorageBackendProgressCallback progress)
{
    if ((!options.generator && !options.packedSource) || options.rows == 0 || options.columns == 0 || options.elementSize == 0 || options.rowTileSize == 0 || options.columnTileSize == 0 || (options.packedTiles && (options.superTileRows == 0 || options.superTileColumns == 0)))
        throw std::invalid_argument("Invalid tiled storage creation options");

    const auto tileGenerator = options.generator ? options.generator : TiledStorageBackendCreationOptions::TileGenerator([](std::uint64_t, std::uint64_t, std::uint64_t, std::uint64_t) -> QByteArray {
        throw std::logic_error("Packed tile source has no staging loader");
    });
    const auto storage = mv::pointdata::tiled::PointDataTiledStorage::fromGenerator(options.rows, options.columns, options.elementSize, options.rowTileSize, options.columnTileSize, [generator = tileGenerator](const mv::pointdata::tiled::TileDescriptor& descriptor) {
        return generator(descriptor.rowOffset, descriptor.columnOffset, descriptor.rows, descriptor.columns);
    }, std::move(progress), options.materializeGeneratedTiles, options.loaderThreadSafe, options.materializationWorkerCount, options.materializationChunkTileRows);

    storage->setCacheCapacity(options.cacheCapacity);
    storage->setCacheCapacityBytes(options.cacheCapacityBytes);
    storage->setPackedTiles(options.packedTiles, options.superTileRows, options.superTileColumns);
    storage->setLoaderThreadSafe(options.loaderThreadSafe);

    if (options.packedSource)
    {
        const auto& packedSource = *options.packedSource;
        if (packedSource.offsets.size() != packedSource.compressedSizes.size() || packedSource.offsets.size() != packedSource.uncompressedSizes.size() || packedSource.offsets.size() != packedSource.superTileIndices.size())
            throw std::invalid_argument("Packed tile source metadata has inconsistent lengths");

        std::vector<mv::util::PackedBlobBlock> blocks;
        blocks.reserve(packedSource.offsets.size());
        for (std::size_t index = 0; index < packedSource.offsets.size(); ++index)
            blocks.push_back({ packedSource.offsets[index], packedSource.compressedSizes[index], packedSource.uncompressedSizes[index] });

        storage->setPackedTileSource(packedSource.filePaths, packedSource.codecName, blocks, packedSource.superTileIndices, packedSource.sourceLifetime, options.generator ? [generator = options.generator](const mv::pointdata::tiled::TileDescriptor& descriptor) {
            return generator(descriptor.rowOffset, descriptor.columnOffset, descriptor.rows, descriptor.columns);
        } : mv::pointdata::tiled::PointDataTiledStorage::TileLoader {});
    }

    return storage;
}

std::shared_ptr<mv::pointdata::tiled::PointDataTiledStorage> TiledPointsStorageBackend::getStorage() const
{
    return _storage;
}

void TiledPointsStorageBackend::setStorage(std::shared_ptr<mv::pointdata::tiled::PointDataTiledStorage> storage)
{
    _storage = std::move(storage);
}

void TiledPointsStorageBackend::setLoaderThreadSafe(const bool threadSafe)
{
    if (_storage)
        _storage->setLoaderThreadSafe(threadSafe);
}

void TiledPointsStorageBackend::persist(StorageBackendProgressCallback progress)
{
    if (!_storage)
        return;

    const auto serializedStorage = mv::pointdata::tiled::PointDataTiledSerialization::toVariantMap(*_storage, progress);
    _storage = mv::pointdata::tiled::PointDataTiledSerialization::fromVariantMap(serializedStorage, _storage->rows(), _storage->columns(), _storage->elementSize());
}

mv::workflow::UniqueWorkflowPlan TiledPointsStorageBackend::persistWorkflow()
{
    if (!_storage)
        return {};

    const auto storage = _storage;
    auto plan = std::make_unique<mv::workflow::WorkflowPlan>("Persist tiled point data");
    const auto serializeStage = plan->addNestedWorkflowStage("Serialize tiled point data", [storage](const mv::workflow::WorkflowPlan::Job&, const mv::workflow::SharedWorkflowExecutionContext&) {
        return mv::pointdata::tiled::PointDataTiledSerialization::toVariantMapWorkflow(*storage);
    });

    plan->addSequentialStage("Install persisted tiled storage", [this, storage, serializeStage](const mv::workflow::WorkflowPlan::Job&, const mv::workflow::SharedWorkflowExecutionContext& executionContext) {
        const auto serializedStorage = executionContext->takeOutput(serializeStage).toMap();
        _storage = mv::pointdata::tiled::PointDataTiledSerialization::fromVariantMap(serializedStorage, storage->rows(), storage->columns(), storage->elementSize());
    }, mv::workflow::WorkflowPlan::JobThreadAffinity::GuiThread);

    return plan;
}

StorageBackendType TiledPointsStorageBackend::getType() const
{
    return StorageBackendType::Tiled;
}

StorageBackendCapabilities TiledPointsStorageBackend::getCapabilities() const
{
    return getStorageBackendCapabilities(StorageBackendType::Tiled);
}

std::unique_ptr<mv::gui::StorageBackendConversionAction> TiledPointsStorageBackend::createConversionAction(const StorageBackendType targetType, QObject* parent) const
{
    if (targetType == StorageBackendType::Tiled && _storage)
    {
        pointdata::storage::StorageBackendConfiguration configuration;
        configuration.rowTileSize    = _storage->rowTileSize();
        configuration.columnTileSize = _storage->columnTileSize();
        configuration.cacheCapacity  = _storage->cacheCapacity();
        configuration.cacheCapacityBytes = _storage->cacheCapacityBytes();
        configuration.packedTiles    = _storage->packedTiles();
        configuration.superTileRows  = _storage->superTileRows();
        configuration.superTileColumns = _storage->superTileColumns();
        return std::make_unique<mv::gui::TiledStorageBackendConversionAction>(parent, _storage->rows(), _storage->columns(), _storage->elementSize(), configuration, true, true, _elementTypeIndex);
    }

    if (targetType == StorageBackendType::Dense && _storage)
        return std::make_unique<mv::gui::DenseStorageBackendConversionAction>(parent, _storage->rows(), _storage->columns(), _elementTypeIndex);

    if (targetType == StorageBackendType::Sparse && _storage)
    {
        const auto sourceTileRows = std::max<std::size_t>(1, _storage->rowTileSize());
        const auto chunkRows = sourceTileRows > 65536 / 3 ? 65536 : sourceTileRows * 3;
        return std::make_unique<mv::gui::SparseStorageBackendConversionAction>(parent, _storage->rows(), _storage->columns(), _elementTypeIndex, false, 4, chunkRows);
    }

    return {};
}

void TiledPointsStorageBackend::applyConfiguration(const StorageBackendConfigurationOptionsPtr& options)
{
    const auto configuration = std::dynamic_pointer_cast<const StorageBackendConfiguration>(options);

    if (!configuration || !_storage)
        throw std::invalid_argument("Missing tiled storage configuration");

    _storage->setCacheCapacity(configuration->cacheCapacity);
    _storage->setCacheCapacityBytes(configuration->cacheCapacityBytes);
    _storage->setPackedTiles(configuration->packedTiles, configuration->superTileRows, configuration->superTileColumns);
}

std::uint64_t TiledPointsStorageBackend::getNumPoints() const
{
    return _storage ? _storage->rows() : 0;
}

std::uint64_t TiledPointsStorageBackend::getNumDimensions() const
{
    return _storage ? _storage->columns() : 0;
}

std::size_t TiledPointsStorageBackend::getElementSize() const
{
    return _storage ? _storage->elementSize() : 0;
}

std::uint32_t TiledPointsStorageBackend::getElementTypeIndex() const
{
    return _elementTypeIndex;
}

std::uint64_t TiledPointsStorageBackend::getStorageSizeBytes() const
{
    if (!_storage)
        return 0;

    if (_storage->rows() != 0 && _storage->columns() > std::numeric_limits<std::uint64_t>::max() / _storage->rows())
        throw std::overflow_error("Tiled point-data dimensions overflow the element count");
    const auto elementCount = _storage->rows() * _storage->columns();
    if (_storage->elementSize() != 0 && elementCount > std::numeric_limits<std::uint64_t>::max() / _storage->elementSize())
        throw std::overflow_error("Tiled point-data byte size exceeds the supported range");
    return elementCount * _storage->elementSize();
}

PointDataAccessView TiledPointsStorageBackend::getRow(const std::uint64_t row) const
{
    if (!_storage || row >= _storage->rows())
        return {};

    if (_storage->elementSize() == 0 || _storage->columns() > std::numeric_limits<std::size_t>::max() / _storage->elementSize())
        throw std::overflow_error("Tiled point-data row is too large");

    auto values = std::make_shared<std::vector<std::uint8_t>>(static_cast<std::size_t>(_storage->columns() * _storage->elementSize()));

    for (std::uint64_t column = 0; column < _storage->columns();)
    {
        const auto tile = _storage->acquire(row / _storage->rowTileSize(), column / _storage->columnTileSize());
        if (!tile || tile.columns() == 0 || tile.rows() == 0)
            throw std::runtime_error("Unable to load tiled point-data row");

        const auto segmentEnd = std::min(_storage->columns(), tile.columnOffset() + tile.columns());
        const auto source = static_cast<const std::uint8_t*>(tile.data()) + (row - tile.rowOffset()) * tile.columns() * tile.elementSize();
        const auto destinationOffset = column * _storage->elementSize();
        const auto byteCount = (segmentEnd - column) * _storage->elementSize();
        std::memcpy(values->data() + destinationOffset, source, static_cast<std::size_t>(byteCount));
        column = segmentEnd;
    }

    PointDataAccessView view;
    view.data              = reinterpret_cast<const char*>(values->data());
    view.rows              = 1;
    view.columns           = _storage->columns();
    view.rowStrideBytes    = _storage->columns() * _storage->elementSize();
    view.elementTypeIndex  = _elementTypeIndex;
    view.elementSizeBytes  = static_cast<std::uint32_t>(_storage->elementSize());
    view.lifetime          = std::shared_ptr<const void>(values, values->data());
    return view;
}

void TiledPointsStorageBackend::visitRange(const std::uint64_t firstRow, const std::uint64_t rowCount, const std::uint64_t firstColumn, const std::uint64_t columnCount, const PointDataAccessVisitor& visitor) const
{
    if (!_storage || !visitor || firstRow > getNumPoints() || rowCount > getNumPoints() - firstRow || firstColumn > getNumDimensions() || columnCount > getNumDimensions() - firstColumn)
        throw std::out_of_range("Point-data range is invalid");

    if (rowCount == 0 || columnCount == 0)
        return;

    for (std::uint64_t row = firstRow; row < firstRow + rowCount; ++row)
    {
        std::uint64_t column = firstColumn;
        const auto endColumn = firstColumn + columnCount;

        while (column < endColumn)
        {
            const auto tile = _storage->acquire(row / _storage->rowTileSize(), column / _storage->columnTileSize());
            if (!tile || tile.columns() == 0 || tile.rows() == 0)
                throw std::runtime_error("Unable to load tiled point-data payload for range access");

            const auto segmentEnd = std::min(endColumn, tile.columnOffset() + tile.columns());

            visitor(makeTileAccessView(tile, row - tile.rowOffset(), column - tile.columnOffset(), 1, segmentEnd - column, _elementTypeIndex));
            column = segmentEnd;
        }
    }
}

float TiledPointsStorageBackend::getValueAt(const std::uint64_t flatIndex) const
{
    return _storage ? _storage->valueAt(flatIndex, _elementTypeIndex) : 0.0f;
}

void TiledPointsStorageBackend::materialize(char* destination, StorageBackendProgressCallback progress) const
{
    if (destination == nullptr)
        return;
    if (!_storage)
        throw std::logic_error("Tiled point-data storage is unavailable for materialization");

    _storage->materialize(destination, progress);
}

}
