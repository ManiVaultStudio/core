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
#include <exception>
#include <limits>
#include <mutex>
#include <stdexcept>
#include <utility>

#include <QFutureSynchronizer>
#include <QThreadPool>
#include <QtConcurrent/QtConcurrentRun>

#include <workflow/WorkflowPlan.h>
#include <workflow/WorkflowExecutionContext.h>

namespace mv::pointdata::storage
{

namespace
{

using TileCoordinate = std::pair<std::uint64_t, std::uint64_t>;

constexpr std::size_t maxConcurrentTileLoads = 8;
constexpr std::size_t maxTilesPerLoadBatch = maxConcurrentTileLoads;

QThreadPool& tileLoadThreadPool()
{
    static QThreadPool pool;
    static const auto configured = []() {
        pool.setMaxThreadCount(static_cast<int>(maxConcurrentTileLoads));
        pool.setExpiryTimeout(-1);
        return true;
    }();
    static_cast<void>(configured);
    return pool;
}

std::vector<mv::pointdata::tiled::TileView> acquireTiles(const mv::pointdata::tiled::PointDataTiledStorage& storage, const std::vector<TileCoordinate>& coordinates)
{
    std::vector<mv::pointdata::tiled::TileView> tiles(coordinates.size());
    if (coordinates.size() == 1)
    {
        tiles.front() = storage.acquire(coordinates.front().first, coordinates.front().second);
        return tiles;
    }

    std::exception_ptr failure;
    std::mutex failureMutex;
    QFutureSynchronizer<void> futures;

    for (std::size_t index = 0; index < coordinates.size(); ++index)
    {
        futures.addFuture(QtConcurrent::run(&tileLoadThreadPool(), [&storage, &coordinates, &tiles, &failure, &failureMutex, index]() {
            try
            {
                tiles[index] = storage.acquire(coordinates[index].first, coordinates[index].second);
            }
            catch (...)
            {
                std::lock_guard lock(failureMutex);
                if (!failure)
                    failure = std::current_exception();
            }
        }));
    }

    futures.waitForFinished();

    if (failure)
        std::rethrow_exception(failure);

    return tiles;
}

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

void visitSelectedColumns(const mv::pointdata::tiled::PointDataTiledStorage& storage, const std::uint32_t elementTypeIndex, const std::vector<std::uint64_t>& columnIds, const std::vector<std::uint64_t>* selectedRows, const PointDataAccessVisitor& visitor)
{
    if (!visitor)
        throw std::invalid_argument("Point-data visitor is invalid");

    for (const auto column : columnIds)
    {
        if (column >= storage.columns())
            throw std::out_of_range("Point-data column ID is out of bounds");
    }

    const auto rowCount = selectedRows ? selectedRows->size() : static_cast<std::size_t>(storage.rows());
    if (columnIds.empty())
    {
        if (selectedRows)
        {
            for (const auto row : *selectedRows)
            {
                if (row >= storage.rows())
                    throw std::out_of_range("Point-data row ID is out of bounds");
            }
        }
        return;
    }
    if (rowCount == 0)
        return;

    std::vector<std::uint64_t> tileColumns;
    tileColumns.reserve(columnIds.size());
    for (const auto column : columnIds)
        tileColumns.push_back(column / storage.columnTileSize());
    std::sort(tileColumns.begin(), tileColumns.end());
    tileColumns.erase(std::unique(tileColumns.begin(), tileColumns.end()), tileColumns.end());

    const auto maxTileRowsPerBatch = std::max<std::size_t>(1, maxTilesPerLoadBatch / tileColumns.size());
    for (std::size_t rowBatchStart = 0; rowBatchStart < rowCount;)
    {
        std::vector<std::uint64_t> tileRows;
        tileRows.reserve(maxTileRowsPerBatch);
        auto rowBatchEnd = rowBatchStart;
        while (rowBatchEnd < rowCount)
        {
            const auto row = selectedRows ? selectedRows->at(rowBatchEnd) : static_cast<std::uint64_t>(rowBatchEnd);
            if (row >= storage.rows())
                throw std::out_of_range("Point-data row ID is out of bounds");

            const auto tileRow = row / storage.rowTileSize();
            const auto tileRowExists = std::find(tileRows.begin(), tileRows.end(), tileRow) != tileRows.end();
            if (!tileRowExists && tileRows.size() >= maxTileRowsPerBatch)
                break;
            if (!tileRowExists)
                tileRows.push_back(tileRow);
            ++rowBatchEnd;
        }
        std::sort(tileRows.begin(), tileRows.end());
        tileRows.erase(std::unique(tileRows.begin(), tileRows.end()), tileRows.end());

        std::vector<TileCoordinate> coordinates;
        coordinates.reserve(tileRows.size() * tileColumns.size());
        for (const auto tileRow : tileRows)
        {
            for (const auto tileColumn : tileColumns)
                coordinates.emplace_back(tileRow, tileColumn);
        }

        const auto tiles = acquireTiles(storage, coordinates);
        for (auto rowIndex = rowBatchStart; rowIndex < rowBatchEnd; ++rowIndex)
        {
            const auto row = selectedRows ? selectedRows->at(rowIndex) : static_cast<std::uint64_t>(rowIndex);
            const auto tileRow = row / storage.rowTileSize();
            const auto tileRowPosition = static_cast<std::size_t>(std::lower_bound(tileRows.begin(), tileRows.end(), tileRow) - tileRows.begin());

            for (std::size_t columnIndex = 0; columnIndex < columnIds.size();)
            {
                auto runEnd = columnIndex + 1;
                while (runEnd < columnIds.size() && columnIds[runEnd - 1] != std::numeric_limits<std::uint64_t>::max() && columnIds[runEnd] == columnIds[runEnd - 1] + 1)
                    ++runEnd;

                auto currentColumn = columnIds[columnIndex];
                while (columnIndex < runEnd)
                {
                    const auto tileColumn = currentColumn / storage.columnTileSize();
                    const auto tileColumnPosition = static_cast<std::size_t>(std::lower_bound(tileColumns.begin(), tileColumns.end(), tileColumn) - tileColumns.begin());
                    const auto& tile = tiles[tileRowPosition * tileColumns.size() + tileColumnPosition];
                    if (!tile || tile.columns() == 0 || tile.rows() == 0)
                        throw std::runtime_error("Unable to load tiled point-data payload for selected-column access");

                    const auto segmentEnd = std::min(columnIds[runEnd - 1] + 1, tile.columnOffset() + tile.columns());
                    visitor(makeTileAccessView(tile, row - tile.rowOffset(), currentColumn - tile.columnOffset(), 1, segmentEnd - currentColumn, elementTypeIndex));
                    while (columnIndex < runEnd && columnIds[columnIndex] < segmentEnd)
                        ++columnIndex;
                    currentColumn = segmentEnd;
                }
            }
        }

        rowBatchStart = rowBatchEnd;
    }
}

void visitSelectedRows(const mv::pointdata::tiled::PointDataTiledStorage& storage, const std::uint32_t elementTypeIndex, const std::vector<std::uint64_t>& rowIds, const std::uint64_t firstColumn, const std::uint64_t columnCount, const PointDataAccessVisitor& visitor)
{
    if (!visitor)
        throw std::invalid_argument("Point-data visitor is invalid");
    if (firstColumn > storage.columns() || columnCount > storage.columns() - firstColumn)
        throw std::out_of_range("Point-data row range is invalid");
    if (rowIds.empty())
        return;

    if (columnCount == 0)
    {
        for (const auto row : rowIds)
        {
            if (row >= storage.rows())
                throw std::out_of_range("Point-data row ID is out of bounds");
        }
        return;
    }

    const auto firstTileColumn = firstColumn / storage.columnTileSize();
    const auto lastTileColumn = (firstColumn + columnCount - 1) / storage.columnTileSize();
    const auto tileColumnCount = lastTileColumn - firstTileColumn + 1;
    const auto maxTileRowsPerBatch = std::max<std::size_t>(1, maxTilesPerLoadBatch / static_cast<std::size_t>(tileColumnCount));
    const auto endColumn = firstColumn + columnCount;

    for (std::size_t rowBatchStart = 0; rowBatchStart < rowIds.size();)
    {
        std::vector<std::uint64_t> tileRows;
        tileRows.reserve(maxTileRowsPerBatch);
        auto rowBatchEnd = rowBatchStart;
        while (rowBatchEnd < rowIds.size())
        {
            const auto row = rowIds[rowBatchEnd];
            if (row >= storage.rows())
                throw std::out_of_range("Point-data row ID is out of bounds");

            const auto tileRow = row / storage.rowTileSize();
            const auto tileRowExists = std::find(tileRows.begin(), tileRows.end(), tileRow) != tileRows.end();
            if (!tileRowExists && tileRows.size() >= maxTileRowsPerBatch)
                break;
            if (!tileRowExists)
                tileRows.push_back(tileRow);
            ++rowBatchEnd;
        }
        std::sort(tileRows.begin(), tileRows.end());
        tileRows.erase(std::unique(tileRows.begin(), tileRows.end()), tileRows.end());

        std::vector<TileCoordinate> coordinates;
        coordinates.reserve(tileRows.size() * static_cast<std::size_t>(tileColumnCount));
        for (const auto tileRow : tileRows)
        {
            for (auto tileColumn = firstTileColumn; tileColumn <= lastTileColumn; ++tileColumn)
                coordinates.emplace_back(tileRow, tileColumn);
        }

        const auto tiles = acquireTiles(storage, coordinates);
        for (auto rowIndex = rowBatchStart; rowIndex < rowBatchEnd; ++rowIndex)
        {
            const auto row = rowIds[rowIndex];
            const auto tileRow = row / storage.rowTileSize();
            const auto tileRowPosition = static_cast<std::size_t>(std::lower_bound(tileRows.begin(), tileRows.end(), tileRow) - tileRows.begin());
            auto column = firstColumn;
            while (column < endColumn)
            {
                const auto tileColumn = column / storage.columnTileSize();
                const auto tileColumnPosition = static_cast<std::size_t>(tileColumn - firstTileColumn);
                const auto& tile = tiles[tileRowPosition * static_cast<std::size_t>(tileColumnCount) + tileColumnPosition];
                if (!tile || tile.columns() == 0 || tile.rows() == 0)
                    throw std::runtime_error("Unable to load tiled point-data payload for selected-row access");

                const auto segmentEnd = std::min(endColumn, tile.columnOffset() + tile.columns());
                visitor(makeTileAccessView(tile, row - tile.rowOffset(), column - tile.columnOffset(), 1, segmentEnd - column, elementTypeIndex));
                column = segmentEnd;
            }
        }

        rowBatchStart = rowBatchEnd;
    }
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
    storage->setSerializationCodecName(options.tileCodecName);
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

bool TiledPointsStorageBackend::supportsProjectResourceStaging() const
{
    return _storage && _storage->packedTiles();
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
        configuration.tileCodecName = _storage->serializationCodecName();
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
    _storage->setSerializationCodecName(configuration->tileCodecName);
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

    const auto tileColumnCount = _storage->tileColumnCount();
    for (std::uint64_t firstTileColumn = 0; firstTileColumn < tileColumnCount; firstTileColumn += maxTilesPerLoadBatch)
    {
        const auto batchSize = std::min<std::uint64_t>(maxTilesPerLoadBatch, tileColumnCount - firstTileColumn);
        std::vector<TileCoordinate> coordinates;
        coordinates.reserve(static_cast<std::size_t>(batchSize));
        for (std::uint64_t tileColumn = firstTileColumn; tileColumn < firstTileColumn + batchSize; ++tileColumn)
            coordinates.emplace_back(row / _storage->rowTileSize(), tileColumn);

        const auto tiles = acquireTiles(*_storage, coordinates);
        for (const auto& tile : tiles)
        {
            if (!tile || tile.columns() == 0 || tile.rows() == 0)
                throw std::runtime_error("Unable to load tiled point-data row");

            const auto source = static_cast<const std::uint8_t*>(tile.data()) + (row - tile.rowOffset()) * tile.columns() * tile.elementSize();
            const auto destinationOffset = tile.columnOffset() * _storage->elementSize();
            const auto byteCount = tile.columns() * _storage->elementSize();
            std::memcpy(values->data() + destinationOffset, source, static_cast<std::size_t>(byteCount));
        }
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

void TiledPointsStorageBackend::visitRows(const std::vector<std::uint64_t>& rowIds, const PointDataAccessVisitor& visitor) const
{
    if (!_storage)
        throw std::logic_error("Tiled point-data storage is unavailable");

    visitSelectedRows(*_storage, _elementTypeIndex, rowIds, 0, _storage->columns(), visitor);
}

void TiledPointsStorageBackend::visitRange(const std::vector<std::uint64_t>& rowIds, const std::uint64_t firstColumn, const std::uint64_t columnCount, const PointDataAccessVisitor& visitor) const
{
    if (!_storage)
        throw std::logic_error("Tiled point-data storage is unavailable");

    visitSelectedRows(*_storage, _elementTypeIndex, rowIds, firstColumn, columnCount, visitor);
}

void TiledPointsStorageBackend::visitColumns(const std::vector<std::uint64_t>& columnIds, const PointDataAccessVisitor& visitor) const
{
    if (!_storage)
        throw std::logic_error("Tiled point-data storage is unavailable");

    visitSelectedColumns(*_storage, _elementTypeIndex, columnIds, nullptr, visitor);
}

void TiledPointsStorageBackend::visitColumns(const std::vector<std::uint64_t>& columnIds, const std::vector<std::uint64_t>& rowIds, const PointDataAccessVisitor& visitor) const
{
    if (!_storage)
        throw std::logic_error("Tiled point-data storage is unavailable");

    visitSelectedColumns(*_storage, _elementTypeIndex, columnIds, &rowIds, visitor);
}

void TiledPointsStorageBackend::visitRange(const std::uint64_t firstRow, const std::uint64_t rowCount, const std::uint64_t firstColumn, const std::uint64_t columnCount, const PointDataAccessVisitor& visitor) const
{
    if (!_storage || !visitor || firstRow > getNumPoints() || rowCount > getNumPoints() - firstRow || firstColumn > getNumDimensions() || columnCount > getNumDimensions() - firstColumn)
        throw std::out_of_range("Point-data range is invalid");

    if (rowCount == 0 || columnCount == 0)
        return;

    const auto firstTileRow = firstRow / _storage->rowTileSize();
    const auto lastTileRow = (firstRow + rowCount - 1) / _storage->rowTileSize();
    const auto firstTileColumn = firstColumn / _storage->columnTileSize();
    const auto lastTileColumn = (firstColumn + columnCount - 1) / _storage->columnTileSize();
    const auto tileColumnCount = lastTileColumn - firstTileColumn + 1;
    const auto tileRowBatchSize = std::max<std::uint64_t>(1, maxTilesPerLoadBatch / tileColumnCount);
    const auto endColumn = firstColumn + columnCount;

    for (auto tileRowBatchStart = firstTileRow; tileRowBatchStart <= lastTileRow;)
    {
        const auto remainingTileRows = lastTileRow + 1 - tileRowBatchStart;
        const auto tileRowBatchEnd = tileRowBatchStart + std::min(remainingTileRows, tileRowBatchSize);
        std::vector<TileCoordinate> coordinates;
        coordinates.reserve(static_cast<std::size_t>((tileRowBatchEnd - tileRowBatchStart) * tileColumnCount));

        for (auto tileRow = tileRowBatchStart; tileRow < tileRowBatchEnd; ++tileRow)
            for (auto tileColumn = firstTileColumn; tileColumn <= lastTileColumn; ++tileColumn)
                coordinates.emplace_back(tileRow, tileColumn);

        const auto tiles = acquireTiles(*_storage, coordinates);

        const auto batchFirstRow = tileRowBatchStart * _storage->rowTileSize();
        const auto batchEndRow = tileRowBatchEnd > _storage->rows() / _storage->rowTileSize() ? _storage->rows() : tileRowBatchEnd * _storage->rowTileSize();
        for (auto row = std::max(firstRow, batchFirstRow); row < std::min(firstRow + rowCount, batchEndRow); ++row)
        {
            auto column = firstColumn;
            while (column < endColumn)
            {
                const auto tileColumn = column / _storage->columnTileSize();
                const auto tileIndex = static_cast<std::size_t>((row / _storage->rowTileSize() - tileRowBatchStart) * tileColumnCount + tileColumn - firstTileColumn);
                const auto& tile = tiles[tileIndex];
                if (!tile || tile.columns() == 0 || tile.rows() == 0)
                    throw std::runtime_error("Unable to load tiled point-data payload for range access");

                const auto segmentEnd = std::min(endColumn, tile.columnOffset() + tile.columns());

                visitor(makeTileAccessView(tile, row - tile.rowOffset(), column - tile.columnOffset(), 1, segmentEnd - column, _elementTypeIndex));
                column = segmentEnd;
            }
        }

        tileRowBatchStart = tileRowBatchEnd;
    }
}

float TiledPointsStorageBackend::getValueAt(const std::uint64_t flatIndex) const
{
    return _storage ? _storage->valueAt(flatIndex, _elementTypeIndex) : 0.0f;
}

void TiledPointsStorageBackend::materialize(char* destination, StorageBackendProgressCallback progress) const
{
    if (!_storage)
        throw std::logic_error("Tiled point-data storage is unavailable for materialization");

    const auto tileRows = _storage->tileRowCount();
    const auto tileColumns = _storage->tileColumnCount();
    const auto totalTiles = tileRows * tileColumns;
    if (!destination && totalTiles != 0)
        throw std::invalid_argument("Tiled materialization destination is null");

    if (progress)
        progress(0.0f);

    const auto rowBatchSize = tileColumns == 0 ? 1 : std::max<std::uint64_t>(1, maxTilesPerLoadBatch / tileColumns);
    const auto progressInterval = std::max<std::uint64_t>(1, totalTiles / 100);
    std::uint64_t completedTiles = 0;

    for (std::uint64_t tileRowBatchStart = 0; tileRowBatchStart < tileRows;)
    {
        const auto tileRowBatchEnd = tileRowBatchStart + std::min(tileRows - tileRowBatchStart, rowBatchSize);
        std::vector<TileCoordinate> coordinates;
        coordinates.reserve(static_cast<std::size_t>((tileRowBatchEnd - tileRowBatchStart) * tileColumns));
        for (auto tileRow = tileRowBatchStart; tileRow < tileRowBatchEnd; ++tileRow)
        {
            for (std::uint64_t tileColumn = 0; tileColumn < tileColumns; ++tileColumn)
                coordinates.emplace_back(tileRow, tileColumn);
        }

        const auto tiles = acquireTiles(*_storage, coordinates);
        for (std::size_t index = 0; index < tiles.size(); ++index)
        {
            const auto& tile = tiles[index];
            if (!tile)
                throw std::runtime_error("Unable to load tiled point-data payload for materialization");

            for (std::uint64_t row = 0; row < tile.rows(); ++row)
            {
                std::memcpy(
                    destination + ((tile.rowOffset() + row) * _storage->columns() + tile.columnOffset()) * _storage->elementSize(),
                    static_cast<const char*>(tile.data()) + row * tile.columns() * _storage->elementSize(),
                    static_cast<std::size_t>(tile.columns() * _storage->elementSize()));
            }

            ++completedTiles;
            if (progress && (completedTiles == 1 || completedTiles == totalTiles || completedTiles % progressInterval == 0))
                progress(static_cast<float>(completedTiles) / static_cast<float>(totalTiles));
        }

        tileRowBatchStart = tileRowBatchEnd;
    }

    if (progress && totalTiles == 0)
        progress(1.0f);
}

}
