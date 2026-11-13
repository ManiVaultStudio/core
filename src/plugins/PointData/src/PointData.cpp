// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PointData.h"
#include "PointDataLegacySerialization.h"

#include "DimensionsPickerAction.h"
#include "actions/InfoAction.h"
#include "actions/PointsAction.h"
#include "DimensionNamesSerializer.h"
#include "storage/dense/DensePointsStorageBackend.h"
#include "storage/interface/PointDataNumericConversion.h"
#include "storage/interface/StorageBackendConversionAction.h"
#include "storage/sparse/SparseStorageBackendConversionAction.h"
#include "storage/sparse/SparsePointsStorageBackend.h"
#include "storage/tiled/PointDataTiledSerialization.h"
#include "storage/tiled/PointDataTiledStorage.h"
#include "storage/tiled/TiledPointsStorageBackend.h"

#include <Application.h>
#include <Project.h>
#include <ProjectCompressionAction.h>

#include <actions/GroupAction.h>
#include <event/Event.h>
#include <graphics/Vector2f.h>

#include <util/Serialization.h>
#include <util/JSON.h>

#include <workflow/WorkflowContextVariantMap.h>

#include <biovault_bfloat16/biovault_bfloat16.h>

#include <QDebug>
#include <QPointer>
#include <QtCore>

#include <cstring>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <limits>
#include <mutex>
#include <set>
#include <stdexcept>
#include <thread>
#include <type_traits>

Q_PLUGIN_METADATA(IID "studio.manivault.PointData")

// =============================================================================
// PointData
// =============================================================================

using namespace mv;
using namespace mv::plugin;
using namespace mv::util;
using namespace mv::workflow;

namespace
{
    template<typename T>
    bool isNonZeroValue(const char* valueBytes)
    {
        T value;
        std::memcpy(&value, valueBytes, sizeof(T));
        if constexpr (std::is_same_v<T, biovault::bfloat16_t>)
            return static_cast<float>(value) != 0.0f;
        else
            return value != T {};
    }

    bool isNonZeroValue(const std::uint32_t elementTypeIndex, const char* valueBytes)
    {
        switch (elementTypeIndex)
        {
            case 0: return isNonZeroValue<float>(valueBytes);
            case 1: return isNonZeroValue<biovault::bfloat16_t>(valueBytes);
            case 2: return isNonZeroValue<std::int32_t>(valueBytes);
            case 3: return isNonZeroValue<std::uint32_t>(valueBytes);
            case 4: return isNonZeroValue<std::int16_t>(valueBytes);
            case 5: return isNonZeroValue<std::uint16_t>(valueBytes);
            case 6: return isNonZeroValue<std::int8_t>(valueBytes);
            case 7: return isNonZeroValue<std::uint8_t>(valueBytes);
            default: throw std::invalid_argument("Sparse conversion encountered an unsupported element type");
        }
    }

    namespace local
    {
        template<typename outT, typename inT>
        inline outT safe_numeric_cast(const inT value)
        {
            static_assert(std::is_integral_v<inT> && std::is_integral_v<outT>);

            assert(value >= std::numeric_limits<outT>::min() && value <= std::numeric_limits<outT>::max());
            return static_cast<outT>(value);
        }
    }

}

PointData::ElementTypeSpecifier PointData::elementTypeSpecifier(const QString& typeName)
{
    if (typeName == "float32")
        return ElementTypeSpecifier::float32;
    else if (typeName == "bfloat16")
        return ElementTypeSpecifier::bfloat16;
    else if (typeName == "int32")
        return ElementTypeSpecifier::int32;
    else if (typeName == "uint32")
        return ElementTypeSpecifier::uint32;
    else if (typeName == "int16")
        return ElementTypeSpecifier::int16;
    else if (typeName == "uint16")
        return ElementTypeSpecifier::uint16;
    else if (typeName == "int8")
        return ElementTypeSpecifier::int8;
    else if (typeName == "uint8")
        return ElementTypeSpecifier::uint8;

    return ElementTypeSpecifier::float32; // Default to float32 if the type name is not recognized
}

PointData::PointData(mv::plugin::PluginFactory* factory) :
    RawData(factory, PointType),
    _storageBackend(std::make_unique<mv::pointdata::storage::DensePointsStorageBackend>(*this))
{
}

PointData::~PointData() = default;

QString PointData::elementTypeName(const ElementTypeSpecifier elementTypeSpecifier)
{
    switch (elementTypeSpecifier)
    {
        case ElementTypeSpecifier::float32:
            return "float32";

        case ElementTypeSpecifier::bfloat16:
            return "bfloat16";

        case ElementTypeSpecifier::int32:
            return "int32";

        case ElementTypeSpecifier::uint32:
            return "uint32";

        case ElementTypeSpecifier::int16:
            return "int16";

        case ElementTypeSpecifier::uint16:
            return "uint16";

        case ElementTypeSpecifier::int8:
            return "int8";

        case ElementTypeSpecifier::uint8:
            return "uint8";

        default:
            return "unknown";
    }

    return "unknown";
}

std::size_t PointData::getElementTypeSize(const std::uint32_t elementTypeIndex)
{
    switch (elementTypeIndex)
    {
        case 0: return sizeof(ElementTypeAt<0>);
        case 1: return sizeof(ElementTypeAt<1>);
        case 2: return sizeof(ElementTypeAt<2>);
        case 3: return sizeof(ElementTypeAt<3>);
        case 4: return sizeof(ElementTypeAt<4>);
        case 5: return sizeof(ElementTypeAt<5>);
        case 6: return sizeof(ElementTypeAt<6>);
        case 7: return sizeof(ElementTypeAt<7>);
        default: throw std::invalid_argument("Unsupported point-data element type");
    }
}

void PointData::init()
{
}

void PointData::refreshStorageBackend()
{
    _storageBackend = std::make_unique<mv::pointdata::storage::DensePointsStorageBackend>(*this);
}

void PointData::clearStorageBackend()
{
    _storageBackend.reset();
}

mv::Dataset<DatasetImpl> PointData::createDataSet(const QString& guid /*= ""*/) const
{
    return mv::Dataset<DatasetImpl>(new Points(getName(), true, guid));
}

std::uint64_t PointData::getNumPoints() const
{
    const auto dataLock = lockData();

    if (_numDimensions == 0)
    {
        qWarning() << "Number of dimensions is 0 in point data ";
        return 0;
    }

    if (_storageBackend && _storageBackend->getType() != pointdata::storage::StorageBackendType::Dense)
        return _storageBackend->getNumPoints();
    if (_isDense)
        return static_cast<std::uint64_t>(getSizeOfVector() / _numDimensions);
    else
        return _numRows;
}

std::uint64_t PointData::getNumDimensions() const
{
    const auto dataLock = lockData();
    return _numDimensions;
}

std::uint64_t PointData::getNumberOfElements() const
{
    const auto dataLock = lockData();
    if (_storageBackend && _storageBackend->getType() != pointdata::storage::StorageBackendType::Dense)
        return _storageBackend->getNumPoints() * _storageBackend->getNumDimensions();

    return getSizeOfVector();
}


std::uint64_t PointData::getRawDataSize() const
{
    const auto dataLock = lockData();

    if (_storageBackend)
        return _storageBackend->getStorageSizeBytes();
    if (_isDense)
    {
        std::uint64_t elementSize = std::visit([](auto& vec) { return vec.empty() ? 0u : sizeof(vec[0]); }, _variantOfVectors);
        return elementSize * getNumberOfElements();
    }
    else
    {
        return static_cast<std::uint64_t>(_numRows + 4) * sizeof(size_t) + _sparseData.getNumNonZeros() * (sizeof(size_t) + sizeof(float));
    }
}

void* PointData::getDataVoidPtr()
{
    const auto dataLock = lockData();
    materializeStorageBackend();
    return std::visit([](auto& vec) { return (void*)vec.data(); }, _variantOfVectors);
}

const void* PointData::getDataConstVoidPtr() const
{
    const auto dataLock = lockData();
    const_cast<PointData*>(this)->materializeStorageBackend();
    return std::visit([](const auto& vec) { return (const void*)vec.data(); }, _variantOfVectors);
}

pointdata::storage::StorageBackendType PointData::getStorageBackendType() const
{
    const auto lock = lockData();

    return _storageBackend ? _storageBackend->getType() : pointdata::storage::StorageBackendType::Dense;
}

pointdata::storage::StorageBackendCapabilities PointData::getStorageBackendCapabilities() const
{
    const auto lock = lockData();

    return _storageBackend ? _storageBackend->getCapabilities() : 0;
}

const pointdata::storage::PointsStorageBackend* PointData::getStorageBackend() const
{
    const auto lock = lockData();

    return _storageBackend.get();
}

void PointData::initializeStorageBackend(std::unique_ptr<pointdata::storage::PointsStorageBackend> storageBackend)
{
    const auto lock = lockData();
    if (!storageBackend)
        throw std::invalid_argument("Cannot install an empty point-data storage backend");
    if (_numRows != 0 || getSizeOfVector() != 0 || (_storageBackend && _storageBackend->getType() != pointdata::storage::StorageBackendType::Dense))
        throw std::invalid_argument("Point-data storage creation requires an empty dataset");

    const auto elementTypeIndex = storageBackend->getElementTypeIndex();
    if (elementTypeIndex >= 8 || getElementTypeSize(elementTypeIndex) != storageBackend->getElementSize())
        throw std::invalid_argument("Point-data storage backend has an unsupported element type");

    setElementTypeSpecifier(static_cast<ElementTypeSpecifier>(elementTypeIndex));
    _numDimensions = storageBackend->getNumDimensions();
    _numRows = storageBackend->getNumPoints();
    _isDense = true;
    _storageBackend = std::move(storageBackend);
}

std::unique_ptr<mv::gui::StorageBackendConversionAction> PointData::createStorageBackendConversionAction(const pointdata::storage::StorageBackendType targetType, QObject* parent) const
{
    const auto lock = lockData();

    if (!_storageBackend)
        return nullptr;

    auto conversionAction = _storageBackend->createConversionAction(targetType, parent);
    if (!conversionAction)
        return nullptr;

    QPointer<PointData> pointData(const_cast<PointData*>(this));
    conversionAction->setSourceData(_storageBackend->getNumPoints(), _storageBackend->getNumDimensions(), _storageBackend->getElementTypeIndex(), [pointData](const std::uint64_t row) {
        if (!pointData)
            return PointDataAccessView {};

        const auto rowLock = pointData->lockData();
        return pointData->_storageBackend ? pointData->_storageBackend->getRow(row) : PointDataAccessView {};
    });

    return conversionAction;
}

void PointData::convertToStorageBackend(const pointdata::storage::StorageBackendType type, const pointdata::storage::StorageBackendConfiguration& configuration, pointdata::storage::StorageBackendProgressCallback progress)
{
    convertToStorageBackend(type, std::make_shared<const pointdata::storage::StorageBackendConfiguration>(configuration), std::move(progress));
}

void PointData::convertToStorageBackend(const pointdata::storage::StorageBackendType type, const pointdata::storage::StorageBackendConfigurationOptionsPtr options, pointdata::storage::StorageBackendProgressCallback progress)
{
    const auto lock = lockData();
    const auto sourceElementTypeIndex = _storageBackend ? _storageBackend->getElementTypeIndex() : static_cast<std::uint32_t>(getElementTypeSpecifier());
    const auto targetElementTypeIndex = options && options->targetElementTypeIndex ? *options->targetElementTypeIndex : sourceElementTypeIndex;
    const auto targetElementSize = getElementTypeSize(targetElementTypeIndex);
    if (_storageBackend && getElementTypeSize(sourceElementTypeIndex) != _storageBackend->getElementSize())
        throw std::invalid_argument("Source storage backend element size does not match its element type");

    const auto getSourceRow = [this, sourceElementTypeIndex](const std::uint64_t row, PointDataAccessView& view) {
        if (_storageBackend)
        {
            view = _storageBackend->getRow(row);
            if (view.rows != 1 || view.elementTypeIndex != sourceElementTypeIndex || view.columns != _storageBackend->getNumDimensions() || view.elementSizeBytes != getElementTypeSize(sourceElementTypeIndex) || (view.columns != 0 && view.data == nullptr))
                throw std::runtime_error("Source row shape or element type changed during storage conversion");
            return static_cast<const char*>(view.data);
        }

        return std::visit([this, row](const auto& vector) {
            if (_numDimensions != 0 && row > std::numeric_limits<std::size_t>::max() / _numDimensions)
                throw std::overflow_error("Source row offset exceeds the process address space");
            const auto rowOffset = static_cast<std::size_t>(row * _numDimensions);
            if (rowOffset > vector.size() || _numDimensions > vector.size() - rowOffset)
                throw std::runtime_error("Source dense row exceeds the stored matrix data");
            return reinterpret_cast<const char*>(rowOffset == 0 ? vector.data() : vector.data() + rowOffset);
        }, _variantOfVectors);
    };

    if (type == pointdata::storage::StorageBackendType::Tiled)
    {
        const auto creation = std::dynamic_pointer_cast<const pointdata::storage::TiledStorageBackendCreationOptions>(options);

        if (creation)
        {
            if (_numRows != 0 || getSizeOfVector() != 0 || (_storageBackend && _storageBackend->getType() != pointdata::storage::StorageBackendType::Dense))
                throw std::invalid_argument("Point-data storage creation requires an empty dataset");

            auto tiledStorage = pointdata::storage::TiledPointsStorageBackend::create(*creation, progress);

            setElementTypeSpecifier(static_cast<ElementTypeSpecifier>(creation->elementTypeIndex));
            _numDimensions = creation->columns;
            _numRows       = creation->rows;
            _isDense       = true;
            _storageBackend = std::make_unique<pointdata::storage::TiledPointsStorageBackend>(std::move(tiledStorage), static_cast<std::uint32_t>(getElementTypeSpecifier()));

            if (progress)
                progress(1.0f);

            return;
        }

        const auto configuration = std::dynamic_pointer_cast<const pointdata::storage::StorageBackendConfiguration>(options);

        if (!configuration)
            throw std::invalid_argument("Missing tiled storage conversion configuration");

        if (targetElementTypeIndex != sourceElementTypeIndex)
        {
            const auto rows = _storageBackend ? _storageBackend->getNumPoints() : _numRows;
            const auto columns = _storageBackend ? _storageBackend->getNumDimensions() : _numDimensions;
            const auto sourceBackend = _storageBackend.get();
            auto creation = std::make_shared<pointdata::storage::TiledStorageBackendCreationOptions>();
            creation->rows = rows;
            creation->columns = columns;
            creation->elementSize = targetElementSize;
            creation->elementTypeIndex = targetElementTypeIndex;
            creation->rowTileSize = configuration->rowTileSize;
            creation->columnTileSize = configuration->columnTileSize;
            creation->cacheCapacity = configuration->cacheCapacity;
            creation->cacheCapacityBytes = configuration->cacheCapacityBytes;
            creation->packedTiles = configuration->packedTiles;
            creation->materializeGeneratedTiles = true;
            creation->loaderThreadSafe = true;
            creation->superTileRows = configuration->superTileRows;
            creation->superTileColumns = configuration->superTileColumns;
            creation->materializationWorkerCount = configuration->conversionWorkerCount;
            creation->materializationChunkTileRows = configuration->conversionChunkTileRows;
            creation->generator = [this, sourceBackend, sourceElementTypeIndex, targetElementTypeIndex, targetElementSize, columns](const std::uint64_t rowOffset, const std::uint64_t columnOffset, const std::uint64_t tileRows, const std::uint64_t tileColumns) {
                if (columnOffset > columns || tileColumns > columns - columnOffset)
                    throw std::out_of_range("Converted tile columns exceed the source matrix");
                if (targetElementSize == 0 || tileColumns > static_cast<std::uint64_t>(std::numeric_limits<qsizetype>::max()) / targetElementSize)
                    throw std::overflow_error("Converted tiled row exceeds the supported byte size");
                const auto rowBytes = tileColumns * targetElementSize;
                if (rowBytes != 0 && tileRows > static_cast<std::uint64_t>(std::numeric_limits<qsizetype>::max()) / rowBytes)
                    throw std::overflow_error("Converted tiled payload exceeds the supported byte size");
                const auto sourceElementSize = getElementTypeSize(sourceElementTypeIndex);
                if (sourceElementSize == 0 || columnOffset > std::numeric_limits<std::size_t>::max() / sourceElementSize)
                    throw std::overflow_error("Source tile offset exceeds the process address space");

                QByteArray payload(static_cast<qsizetype>(tileRows * rowBytes), Qt::Uninitialized);
                std::vector<std::uint8_t> convertedRow(static_cast<std::size_t>(rowBytes));
                for (std::uint64_t localRow = 0; localRow < tileRows; ++localRow)
                {
                    PointDataAccessView sourceView;
                    const auto* sourceRow = static_cast<const char*>(nullptr);
                    if (sourceBackend)
                    {
                        sourceView = sourceBackend->getRow(rowOffset + localRow);
                        if (sourceView.rows != 1 || sourceView.elementTypeIndex != sourceElementTypeIndex || sourceView.columns != columns || sourceView.elementSizeBytes != getElementTypeSize(sourceElementTypeIndex) || (columns != 0 && sourceView.data == nullptr))
                            throw std::runtime_error("Source row shape or element type changed during tiled conversion");
                        sourceRow = static_cast<const char*>(sourceView.data);
                    }
                    else
                        sourceRow = std::visit([this, row = rowOffset + localRow](const auto& vector) { return reinterpret_cast<const char*>(vector.data() + static_cast<std::size_t>(row * _numDimensions)); }, _variantOfVectors);
                    const auto* sourceTileData = tileColumns == 0 ? sourceRow : sourceRow + columnOffset * sourceElementSize;
                    pointdata::storage::detail::convertNumericRow(sourceElementTypeIndex, targetElementTypeIndex, sourceTileData, tileColumns, convertedRow.data());
                    if (rowBytes != 0)
                        std::memcpy(payload.data() + localRow * rowBytes, convertedRow.data(), static_cast<std::size_t>(rowBytes));
                }

                return payload;
            };

            auto convertedStorage = pointdata::storage::TiledPointsStorageBackend::create(*creation, progress);
            _storageBackend = std::make_unique<pointdata::storage::TiledPointsStorageBackend>(std::move(convertedStorage), targetElementTypeIndex);
            setElementTypeSpecifier(static_cast<ElementTypeSpecifier>(targetElementTypeIndex));
            _numRows = rows;
            _numDimensions = columns;
            _isDense = true;
            return;
        }

        if (const auto sparseStorage = dynamic_cast<const pointdata::storage::SparsePointsStorageBackend*>(_storageBackend.get()))
        {
            auto creation = std::make_shared<pointdata::storage::TiledStorageBackendCreationOptions>();
            creation->rows = sparseStorage->getNumPoints();
            creation->columns = sparseStorage->getNumDimensions();
            creation->elementSize = sparseStorage->getElementSize();
            creation->elementTypeIndex = sparseStorage->getElementTypeIndex();
            creation->rowTileSize = configuration->rowTileSize;
            creation->columnTileSize = configuration->columnTileSize;
            creation->cacheCapacity = configuration->cacheCapacity;
            creation->cacheCapacityBytes = configuration->cacheCapacityBytes;
            creation->packedTiles = configuration->packedTiles;
            creation->materializeGeneratedTiles = true;
            creation->loaderThreadSafe = true;
            creation->superTileRows = configuration->superTileRows;
            creation->superTileColumns = configuration->superTileColumns;
            creation->materializationWorkerCount = configuration->conversionWorkerCount;
            creation->materializationChunkTileRows = configuration->conversionChunkTileRows;
            creation->generator = [sparseStorage, elementSize = creation->elementSize, columns = creation->columns](const std::uint64_t rowOffset, const std::uint64_t columnOffset, const std::uint64_t tileRows, const std::uint64_t tileColumns) {
                if (elementSize == 0 || tileColumns > std::numeric_limits<std::uint64_t>::max() / elementSize)
                    throw std::overflow_error("Sparse-to-tiled payload exceeds the supported byte size");

                const auto rowBytes = tileColumns * elementSize;
                if (rowBytes != 0 && tileRows > static_cast<std::uint64_t>(std::numeric_limits<qsizetype>::max()) / rowBytes)
                    throw std::overflow_error("Sparse-to-tiled payload exceeds the supported byte size");

                QByteArray payload;
                payload.resize(static_cast<qsizetype>(tileRows * rowBytes));
                for (std::uint64_t localRow = 0; localRow < tileRows; ++localRow)
                {
                    const auto row = sparseStorage->getRow(rowOffset + localRow);
                    if (row.columns != columns || row.elementSizeBytes != elementSize)
                        throw std::runtime_error("Sparse row shape changed during tiled conversion");
                    if (columnOffset > row.columns || tileColumns > row.columns - columnOffset || columnOffset > std::numeric_limits<std::size_t>::max() / elementSize)
                        throw std::out_of_range("Sparse row tile columns exceed the source row");
                    if (rowBytes != 0)
                        std::memcpy(payload.data() + localRow * rowBytes, static_cast<const char*>(row.data) + columnOffset * elementSize, static_cast<std::size_t>(rowBytes));
                }

                return payload;
            };

            auto tiledStorage = pointdata::storage::TiledPointsStorageBackend::create(*creation, std::move(progress));
            _storageBackend = std::make_unique<pointdata::storage::TiledPointsStorageBackend>(std::move(tiledStorage), creation->elementTypeIndex);
            return;
        }

        if (const auto* tiledBackend = dynamic_cast<const pointdata::storage::TiledPointsStorageBackend*>(_storageBackend.get()))
        {
            const auto tiledStorage = tiledBackend->getStorage();
            if (!tiledStorage)
                throw std::logic_error("Tiled point-data storage is unavailable");

            std::shared_ptr<mv::pointdata::tiled::PointDataTiledStorage> convertedStorage;
            if (configuration->packedTiles)
            {
                const auto* project = mv::projects().getCurrentProject();
                if (!project)
                    throw std::runtime_error("Unable to convert tiled point data without an active project");

                const auto codec = project->getCompressionAction().createCodec(nullptr);
                const auto codecFactory = [project] { return project->getCompressionAction().createCodec(nullptr); };
                convertedStorage = mv::pointdata::tiled::PointDataTiledStorage::retileToPackedFiles(
                    tiledStorage,
                    configuration->rowTileSize,
                    configuration->columnTileSize,
                    configuration->superTileRows,
                    configuration->superTileColumns,
                    codec,
                    progress,
                    configuration->conversionWorkerCount,
                    configuration->conversionChunkTileRows,
                    codecFactory);
            }
            else
            {
                const auto source = std::shared_ptr<const mv::pointdata::tiled::PointDataTiledStorage>(tiledStorage);
                auto generator = [source](const mv::pointdata::tiled::TileDescriptor& descriptor) {
                    if (descriptor.rows != 0 && descriptor.columns > static_cast<std::uint64_t>(std::numeric_limits<qsizetype>::max()) / descriptor.rows / source->elementSize())
                        throw std::overflow_error("Converted tile exceeds supported byte size");

                    QByteArray payload(static_cast<qsizetype>(descriptor.rows * descriptor.columns * source->elementSize()), Qt::Uninitialized);
                    for (std::uint64_t localRow = 0; localRow < descriptor.rows; ++localRow)
                    {
                        std::uint64_t localColumn = 0;
                        const auto sourceRow = descriptor.rowOffset + localRow;
                        while (localColumn < descriptor.columns)
                        {
                            const auto column = descriptor.columnOffset + localColumn;
                            const auto sourceTile = source->acquire(sourceRow / source->rowTileSize(), column / source->columnTileSize());
                            if (!sourceTile || sourceTile.columns() == 0 || sourceTile.rows() == 0)
                                throw std::runtime_error("Unable to read source tile during storage conversion");

                            const auto sourceRowOffset = sourceRow - sourceTile.rowOffset();
                            const auto sourceColumnOffset = column - sourceTile.columnOffset();
                            if (sourceRowOffset >= sourceTile.rows() || sourceColumnOffset >= sourceTile.columns())
                                throw std::runtime_error("Source tile does not contain the requested conversion range");
                            const auto copyColumns = std::min<std::uint64_t>(descriptor.columns - localColumn, sourceTile.columns() - sourceColumnOffset);
                            const auto sourceOffset = (sourceRowOffset * sourceTile.columns() + sourceColumnOffset) * source->elementSize();
                            const auto targetOffset = (localRow * descriptor.columns + localColumn) * source->elementSize();
                            std::memcpy(payload.data() + targetOffset, static_cast<const char*>(sourceTile.data()) + sourceOffset, static_cast<std::size_t>(copyColumns * source->elementSize()));
                            localColumn += copyColumns;
                        }
                    }

                    return payload;
                };

                convertedStorage = mv::pointdata::tiled::PointDataTiledStorage::fromGenerator(
                    source->rows(),
                    source->columns(),
                    source->elementSize(),
                    configuration->rowTileSize,
                    configuration->columnTileSize,
                    std::move(generator),
                    progress,
                    true,
                    true,
                    configuration->conversionWorkerCount,
                    configuration->conversionChunkTileRows);
                convertedStorage->setCacheCapacity(configuration->cacheCapacity);
                convertedStorage->setCacheCapacityBytes(configuration->cacheCapacityBytes);
                convertedStorage->setPackedTiles(false);
                convertedStorage->setLoaderThreadSafe(true);
            }

            dynamic_cast<pointdata::storage::TiledPointsStorageBackend*>(_storageBackend.get())->setStorage(std::move(convertedStorage));
        }
        else
            makeTiled(configuration->rowTileSize, configuration->columnTileSize, progress, configuration->conversionWorkerCount, configuration->conversionChunkTileRows);

        if (_storageBackend)
            _storageBackend->applyConfiguration(options);

        return;
    }

    if (type == pointdata::storage::StorageBackendType::Dense)
    {
        if (targetElementTypeIndex != sourceElementTypeIndex)
        {
            const auto rows = _storageBackend ? _storageBackend->getNumPoints() : _numRows;
            const auto columns = _storageBackend ? _storageBackend->getNumDimensions() : _numDimensions;
            if (rows != 0 && columns > std::numeric_limits<std::uint64_t>::max() / rows)
                throw std::overflow_error("Dense conversion dimensions overflow the dense element count");
            if (columns > std::numeric_limits<std::size_t>::max() / targetElementSize)
                throw std::overflow_error("Dense conversion row exceeds the process address space");
            const auto elementCount = rows * columns;
            if (elementCount > std::numeric_limits<std::size_t>::max() / targetElementSize)
                throw std::overflow_error("Dense conversion exceeds the process address space");

            VariantOfVectors convertedVectors;
            setIndexOfVariant(convertedVectors, targetElementTypeIndex);
            std::visit([elementCount](auto& vector) { vector.resize(static_cast<std::size_t>(elementCount)); }, convertedVectors);
            auto* destination = static_cast<std::uint8_t*>(std::visit([](auto& vector) { return static_cast<void*>(vector.data()); }, convertedVectors));
            std::vector<std::uint8_t> convertedRow(static_cast<std::size_t>(columns) * targetElementSize);
            for (std::uint64_t row = 0; row < rows; ++row)
            {
                PointDataAccessView sourceView;
                const auto* sourceRow = getSourceRow(row, sourceView);
                pointdata::storage::detail::convertNumericRow(sourceElementTypeIndex, targetElementTypeIndex, sourceRow, columns, convertedRow.data());
                if (!convertedRow.empty())
                    std::memcpy(destination + static_cast<std::size_t>(row * columns) * targetElementSize, convertedRow.data(), convertedRow.size());
                if (progress && (row % std::max<std::uint64_t>(1, rows / 100) == 0))
                    progress(rows == 0 ? 1.0f : static_cast<float>(row) / static_cast<float>(rows));
            }

            _variantOfVectors = std::move(convertedVectors);
            _numRows = rows;
            _numDimensions = columns;
            _isDense = true;
            clearStorageBackend();
            refreshStorageBackend();
            if (progress)
                progress(1.0f);
            return;
        }

        materializeStorageBackend(std::move(progress));

        return;
    }

    if (type == pointdata::storage::StorageBackendType::Sparse)
    {
        const auto configuration = std::dynamic_pointer_cast<const pointdata::storage::SparseStorageBackendConfiguration>(options);
        if (!configuration)
            throw std::invalid_argument("Missing sparse storage conversion configuration");

        const auto rows = _storageBackend ? _storageBackend->getNumPoints() : _numRows;
        const auto columns = _storageBackend ? _storageBackend->getNumDimensions() : _numDimensions;
        const auto elementSize = targetElementSize;
        const auto elementTypeIndex = targetElementTypeIndex;
        const auto conversionStartedAt = std::chrono::steady_clock::now();

        if (rows != 0 && columns > std::numeric_limits<std::uint64_t>::max() / rows)
            throw std::overflow_error("Sparse conversion dimensions overflow the dense element count");
        if (rows >= std::numeric_limits<std::size_t>::max() || elementSize == 0 || columns > std::numeric_limits<std::size_t>::max() / elementSize)
            throw std::overflow_error("Sparse conversion row is too large");

        struct SparseConversionChunk
        {
            std::uint64_t firstRow = 0;
            std::uint64_t rowCount = 0;
            std::vector<std::uint64_t> rowPointers;
            std::vector<std::uint64_t> indices;
            std::vector<std::uint8_t> values;
        };

        const auto chunkRows = std::clamp<std::size_t>(configuration->conversionChunkRows, 1, 65536);
        const auto chunkCount = static_cast<std::size_t>(rows / chunkRows + (rows % chunkRows != 0));
        const auto workerCount = std::min<std::size_t>(std::clamp<std::size_t>(configuration->conversionWorkerCount, 1, 32), std::max<std::size_t>(1, chunkCount));
        std::vector<SparseConversionChunk> chunks(chunkCount);
        const auto rowScanProgressEnd = configuration->columnCompressed ? 0.65f : 0.99f;

        if (progress)
            progress(0.0f);

        std::atomic_size_t nextChunk{ 0 };
        std::atomic_size_t activeWorkers{ 0 };
        std::atomic_size_t maximumConcurrentWorkers{ 0 };
        std::atomic_size_t completedRows{ 0 };
        std::atomic_bool stopWorkers{ false };
        std::atomic_uint64_t aggregateWorkerNanoseconds{ 0 };
        std::mutex progressMutex;
        std::mutex exceptionMutex;
        std::exception_ptr workerException;
        std::size_t lastProgressRows = 0;

        std::vector<std::jthread> workers;
        workers.reserve(workerCount);
        for (std::size_t workerIndex = 0; workerIndex < workerCount; ++workerIndex)
        {
            workers.emplace_back([&] {
                const auto active = activeWorkers.fetch_add(1) + 1;
                auto maximum = maximumConcurrentWorkers.load();
                while (maximum < active && !maximumConcurrentWorkers.compare_exchange_weak(maximum, active))
                {
                }

                const auto workerStartedAt = std::chrono::steady_clock::now();
                try
                {
                    while (!stopWorkers.load())
                    {
                        const auto chunkIndex = nextChunk.fetch_add(1);
                        if (chunkIndex >= chunks.size())
                            break;

                        auto& chunk = chunks[chunkIndex];
                        chunk.firstRow = static_cast<std::uint64_t>(chunkIndex) * chunkRows;
                        chunk.rowCount = std::min<std::uint64_t>(rows - chunk.firstRow, chunkRows);
                        chunk.rowPointers.resize(static_cast<std::size_t>(chunk.rowCount) + 1, 0);
                        std::vector<std::uint8_t> convertedRow(static_cast<std::size_t>(columns) * elementSize);

                        for (std::uint64_t localRow = 0; localRow < chunk.rowCount; ++localRow)
                        {
                            const auto row = chunk.firstRow + localRow;
                            PointDataAccessView sourceView;
                            const auto* sourceRow = getSourceRow(row, sourceView);
                            if (sourceView.data && sourceView.elementSizeBytes != getElementTypeSize(sourceElementTypeIndex))
                                throw std::runtime_error("Source row element size changed during sparse conversion");
                            pointdata::storage::detail::convertNumericRow(sourceElementTypeIndex, elementTypeIndex, sourceRow, columns, convertedRow.data());

                            const auto* rowData = reinterpret_cast<const char*>(convertedRow.data());
                            if (columns != 0 && rowData == nullptr)
                                throw std::runtime_error("Unable to access a source row during sparse conversion");
                            for (std::uint64_t column = 0; column < columns; ++column)
                            {
                                const auto* value = rowData + column * elementSize;
                                if (!isNonZeroValue(elementTypeIndex, value))
                                    continue;

                                chunk.indices.push_back(column);
                                const auto oldSize = chunk.values.size();
                                if (elementSize > std::numeric_limits<std::size_t>::max() - oldSize)
                                    throw std::overflow_error("Sparse conversion value array exceeds the process address space");
                                chunk.values.resize(oldSize + elementSize);
                                std::memcpy(chunk.values.data() + oldSize, value, elementSize);
                            }

                            chunk.rowPointers[static_cast<std::size_t>(localRow + 1)] = chunk.indices.size();
                            const auto complete = completedRows.fetch_add(1) + 1;
                            if (progress && ((complete % std::max<std::uint64_t>(1, rows / 100) == 0) || complete == rows))
                            {
                                const std::lock_guard<std::mutex> lock(progressMutex);
                                if (complete > lastProgressRows)
                                {
                                    lastProgressRows = complete;
                                    progress(rowScanProgressEnd * static_cast<float>(complete) / static_cast<float>(rows));
                                }
                            }
                        }
                    }
                }
                catch (...)
                {
                    stopWorkers.store(true);
                    const std::lock_guard<std::mutex> lock(exceptionMutex);
                    if (!workerException)
                        workerException = std::current_exception();
                }
                aggregateWorkerNanoseconds.fetch_add(static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - workerStartedAt).count()));
                activeWorkers.fetch_sub(1);
            });
        }
        workers.clear(); // jthread destruction joins all conversion workers.
        if (workerException)
            std::rethrow_exception(workerException);

        std::vector<std::uint64_t> rowPointers(static_cast<std::size_t>(rows) + 1, 0);
        std::size_t nonzeroCount = 0;
        for (const auto& chunk : chunks)
        {
            if (chunk.indices.size() > std::numeric_limits<std::size_t>::max() - nonzeroCount)
                throw std::overflow_error("Sparse conversion index array exceeds the process address space");
            nonzeroCount += chunk.indices.size();
        }
        std::vector<std::uint64_t> rowIndices;
        std::vector<std::uint8_t> sparseValues;
        rowIndices.reserve(nonzeroCount);
        if (elementSize != 0 && nonzeroCount > std::numeric_limits<std::size_t>::max() / elementSize)
            throw std::overflow_error("Sparse conversion value array exceeds the process address space");
        sparseValues.reserve(nonzeroCount * elementSize);
        std::uint64_t nonzeroOffset = 0;
        for (const auto& chunk : chunks)
        {
            for (std::uint64_t localRow = 0; localRow < chunk.rowCount; ++localRow)
                rowPointers[static_cast<std::size_t>(chunk.firstRow + localRow + 1)] = nonzeroOffset + chunk.rowPointers[static_cast<std::size_t>(localRow + 1)];
            nonzeroOffset += chunk.indices.size();
            rowIndices.insert(rowIndices.end(), chunk.indices.begin(), chunk.indices.end());
            sparseValues.insert(sparseValues.end(), chunk.values.begin(), chunk.values.end());
        }

        if (progress && rows == 0)
            progress(rowScanProgressEnd);

        auto pointers = std::move(rowPointers);
        auto indices = std::move(rowIndices);
        auto values = std::move(sparseValues);
        auto layout = pointdata::storage::SparseMatrixLayout::RowCompressed;

        if (configuration->columnCompressed)
        {
            if (columns >= std::numeric_limits<std::size_t>::max())
                throw std::overflow_error("Column-compressed sparse storage has too many column pointers");

            std::vector<std::uint64_t> columnPointers(static_cast<std::size_t>(columns) + 1, 0);
            const auto countProgressInterval = std::max<std::size_t>(1, indices.size() / 100);
            for (std::size_t index = 0; index < indices.size(); ++index)
            {
                const auto column = indices[index];
                if (column >= columns)
                    throw std::runtime_error("Sparse conversion produced an out-of-range column index");
                ++columnPointers[static_cast<std::size_t>(column + 1)];
                if (progress && ((index + 1) % countProgressInterval == 0 || index + 1 == indices.size()))
                    progress(0.65f + 0.07f * static_cast<float>(index + 1) / static_cast<float>(indices.size()));
            }
            if (progress && indices.empty())
                progress(0.72f);

            const auto prefixProgressInterval = std::max<std::uint64_t>(1, columns / 100);
            for (std::size_t column = 1; column < columnPointers.size(); ++column)
            {
                columnPointers[column] += columnPointers[column - 1];
                if (progress && (column % prefixProgressInterval == 0 || column + 1 == columnPointers.size()))
                    progress(0.72f + 0.06f * static_cast<float>(column) / static_cast<float>(columns));
            }
            if (progress && columns == 0)
                progress(0.78f);

            auto next = columnPointers;
            std::vector<std::uint64_t> columnIndices(indices.size());
            std::vector<std::uint8_t> columnValues(values.size());
            const auto scatterProgressInterval = std::max<std::uint64_t>(1, rows / 100);
            for (std::uint64_t row = 0; row < rows; ++row)
            {
                for (auto index = pointers[static_cast<std::size_t>(row)]; index < pointers[static_cast<std::size_t>(row + 1)]; ++index)
                {
                    const auto column = indices[static_cast<std::size_t>(index)];
                    const auto destination = next[static_cast<std::size_t>(column)]++;
                    if (destination >= columnIndices.size())
                        throw std::runtime_error("Column-compressed sparse conversion produced an invalid value offset");
                    columnIndices[static_cast<std::size_t>(destination)] = row;
                    std::memcpy(columnValues.data() + static_cast<std::size_t>(destination) * elementSize, values.data() + static_cast<std::size_t>(index) * elementSize, elementSize);
                }
                if (progress && ((row + 1) % scatterProgressInterval == 0 || row + 1 == rows))
                    progress(0.78f + 0.21f * static_cast<float>(row + 1) / static_cast<float>(rows));
            }
            if (progress && rows == 0)
                progress(0.99f);

            pointers = std::move(columnPointers);
            indices = std::move(columnIndices);
            values = std::move(columnValues);
            layout = pointdata::storage::SparseMatrixLayout::ColumnCompressed;
        }

        auto sparseBackend = std::make_unique<pointdata::storage::SparsePointsStorageBackend>(rows, columns, layout, elementTypeIndex, elementSize, std::move(pointers), std::move(indices), std::move(values));
        setElementTypeSpecifier(static_cast<ElementTypeSpecifier>(elementTypeIndex));
        _numDimensions = columns;
        _numRows = rows;
        _isDense = false;
        _storageBackend = std::move(sparseBackend);

        if (progress)
            progress(1.0f);

        const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - conversionStartedAt).count();
        const auto aggregateWorkerMs = aggregateWorkerNanoseconds.load() / 1000000;
        qInfo().noquote() << QStringLiteral("[PointData] Sparse conversion: %1 rows x %2 columns; %3 nonzero values; requested workers %4; maximum concurrent workers %5; aggregate worker time %6 ms; elapsed %7 ms; layout %8")
            .arg(rows)
            .arg(columns)
            .arg(nonzeroCount)
            .arg(std::clamp<std::size_t>(configuration->conversionWorkerCount, 1, 32))
            .arg(maximumConcurrentWorkers.load())
            .arg(aggregateWorkerMs)
            .arg(elapsedMs)
            .arg(configuration->columnCompressed ? QStringLiteral("CSC") : QStringLiteral("CSR"));

        return;
    }

    throw std::invalid_argument("Unsupported point-data storage backend");
}

void PointData::configureStorageBackend(const pointdata::storage::StorageBackendConfigurationOptionsPtr options)
{
    const auto lock = lockData();

    if (!_storageBackend)
        throw std::invalid_argument("Point-data storage backend is unavailable");

    _storageBackend->applyConfiguration(options);
}

void PointData::makeTiled(const std::size_t rowTileSize, const std::size_t columnTileSize, std::function<void(float)> progress, const std::size_t maxWorkerCount, const std::size_t chunkTileRowFactor)
{
    const auto lock = lockData();

    materializeStorageBackend();

    if (_isDense && (!_storageBackend || _storageBackend->getType() == pointdata::storage::StorageBackendType::Dense) && _numDimensions > 0)
    {
        const auto rows = static_cast<std::uint64_t>(getSizeOfVector() / _numDimensions);

        const auto elementSize = std::visit([](const auto& vector) { return sizeof(typename std::decay_t<decltype(vector)>::value_type); }, _variantOfVectors);
        const auto* data = static_cast<const char*>(std::visit([](const auto& vector) { return static_cast<const void*>(vector.data()); }, _variantOfVectors));

        const auto columns = _numDimensions;
        auto generator = [data, columns, elementSize](const mv::pointdata::tiled::TileDescriptor& descriptor) {
            const auto rowBytes = descriptor.columns * elementSize;
            QByteArray payload(static_cast<qsizetype>(descriptor.rows * rowBytes), Qt::Uninitialized);
            for (std::uint64_t localRow = 0; localRow < descriptor.rows; ++localRow)
            {
                const auto sourceOffset = ((descriptor.rowOffset + localRow) * columns + descriptor.columnOffset) * elementSize;
                std::memcpy(payload.data() + localRow * rowBytes, data + sourceOffset, static_cast<std::size_t>(rowBytes));
            }
            return payload;
        };
        const auto elementTypeIndex = static_cast<std::uint32_t>(getElementTypeSpecifier());
        const auto tiledStorage = mv::pointdata::tiled::PointDataTiledStorage::fromGenerator(rows, columns, elementSize, rowTileSize, columnTileSize, std::move(generator), std::move(progress), true, true, maxWorkerCount, chunkTileRowFactor);

        _numRows = rows;
        _storageBackend = std::make_unique<pointdata::storage::TiledPointsStorageBackend>(tiledStorage, elementTypeIndex);
    }
}

namespace
{
PointDataAccessView makeTypedAccessView(const void* source, const std::uint64_t columns, const std::uint32_t elementTypeIndex, const std::uint32_t elementSize)
{
    if (elementSize == 0 || columns > std::numeric_limits<std::size_t>::max() / elementSize)
        throw std::overflow_error("Point-data access row is too large");

    auto bytes = std::make_shared<std::vector<std::uint8_t>>(static_cast<std::size_t>(columns) * elementSize);
    if (!bytes->empty())
        std::memcpy(bytes->data(), source, bytes->size());

    PointDataAccessView view;
    view.data = bytes->data();
    view.rows = 1;
    view.columns = columns;
    view.rowStrideBytes = static_cast<std::uint64_t>(bytes->size());
    view.elementTypeIndex = elementTypeIndex;
    view.elementSizeBytes = elementSize;
    view.lifetime = std::shared_ptr<const void>(bytes, bytes->data());
    return view;
}

}

PointDataAccessView PointData::getDenseRow(const std::uint64_t row) const
{
    if (row >= getNumPoints())
        return {};

    return std::visit([&](const auto& vector) {
        using ValueType = typename std::decay_t<decltype(vector)>::value_type;
        const auto* rowData = _numDimensions == 0 ? nullptr : vector.data() + static_cast<std::size_t>(row * _numDimensions);
        return makeTypedAccessView(rowData, _numDimensions, static_cast<std::uint32_t>(getElementTypeSpecifier()), sizeof(ValueType));
    }, _variantOfVectors);
}

PointDataAccessView PointData::getRow(const std::uint64_t row) const
{
    return _storageBackend ? _storageBackend->getRow(row) : PointDataAccessView{};
}

void PointData::visitRows(const std::uint64_t firstRow, const std::uint64_t rowCount, const PointDataAccessVisitor& visitor) const
{
    if (firstRow > getNumPoints() || rowCount > getNumPoints() - firstRow)
        throw std::out_of_range("Point-data row range is out of bounds");

    for (std::uint64_t row = firstRow; row < firstRow + rowCount; ++row)
        visitRange(row, 1, 0, _numDimensions, visitor);
}

void PointData::visitRows(const std::vector<std::uint64_t>& rowIds, const PointDataAccessVisitor& visitor) const
{
    for (const auto row : rowIds)
        visitRows(row, 1, visitor);
}

void PointData::visitColumns(const std::uint64_t firstColumn, const std::uint64_t columnCount, const PointDataAccessVisitor& visitor) const
{
    if (firstColumn > _numDimensions || columnCount > _numDimensions - firstColumn)
        throw std::out_of_range("Point-data column range is out of bounds");

    for (std::uint64_t row = 0; row < getNumPoints(); ++row)
        visitRange(row, 1, firstColumn, columnCount, visitor);
}

void PointData::visitColumns(const std::vector<std::uint64_t>& columnIds, const PointDataAccessVisitor& visitor) const
{
    std::vector<std::uint64_t> rowIds(getNumPoints());
    std::iota(rowIds.begin(), rowIds.end(), 0);

    visitColumns(columnIds, rowIds, visitor);
}

void PointData::visitColumns(const std::uint64_t firstColumn, const std::uint64_t columnCount, const std::vector<std::uint64_t>& rowIds, const PointDataAccessVisitor& visitor) const
{
    if (firstColumn > _numDimensions || columnCount > _numDimensions - firstColumn)
        throw std::out_of_range("Point-data column range is out of bounds");

    for (const auto row : rowIds)
    {
        if (row >= getNumPoints())
            throw std::out_of_range("Point-data row ID is out of bounds");
        visitRange(row, 1, firstColumn, columnCount, visitor);
    }
}

void PointData::visitColumns(const std::vector<std::uint64_t>& columnIds, const std::vector<std::uint64_t>& rowIds, const PointDataAccessVisitor& visitor) const
{
    if (!visitor)
        throw std::invalid_argument("Point-data visitor is invalid");

    for (const auto column : columnIds)
    {
        if (column >= getNumDimensions())
            throw std::out_of_range("Point-data column ID is out of bounds");
    }

    for (const auto row : rowIds)
    {
        if (row >= getNumPoints())
            throw std::out_of_range("Point-data row ID is out of bounds");

        for (std::size_t index = 0; index < columnIds.size();)
        {
            const auto firstColumn = columnIds[index];
            auto end = index + 1;

            while (end < columnIds.size() && columnIds[end] == columnIds[end - 1] + 1)
                ++end;

            visitRange(row, 1, firstColumn, end - index, visitor);
            index = end;
        }
    }
}

void PointData::visitDenseRange(const std::uint64_t firstRow, const std::uint64_t rowCount, const std::uint64_t firstColumn, const std::uint64_t columnCount, const PointDataAccessVisitor& visitor) const
{
    if (!visitor || firstRow > getNumPoints() || rowCount > getNumPoints() - firstRow ||
        firstColumn > _numDimensions || columnCount > _numDimensions - firstColumn)
        throw std::out_of_range("Point-data range is invalid");
    if (rowCount == 0 || columnCount == 0)
        return;

    for (std::uint64_t row = firstRow; row < firstRow + rowCount; ++row)
    {
        std::visit([&](const auto& vector) {
            using ValueType = typename std::decay_t<decltype(vector)>::value_type;
            const auto* rowData = vector.data() + static_cast<std::size_t>(row * _numDimensions + firstColumn);
            visitor(makeTypedAccessView(rowData, columnCount, static_cast<std::uint32_t>(getElementTypeSpecifier()), sizeof(ValueType)));
        }, _variantOfVectors);
    }
}

void PointData::visitRange(const std::uint64_t firstRow, const std::uint64_t rowCount, const std::uint64_t firstColumn, const std::uint64_t columnCount, const PointDataAccessVisitor& visitor) const
{
    if (_storageBackend)
        _storageBackend->visitRange(firstRow, rowCount, firstColumn, columnCount, visitor);
}

void PointData::setRows(const std::uint64_t firstRow, const std::uint64_t rowCount, const PointDataAccessView& values)
{
    setRange(firstRow, rowCount, 0, getNumDimensions(), values);
}

void PointData::setColumns(const std::uint64_t firstColumn, const std::uint64_t columnCount, const PointDataAccessView& values)
{
    setRange(0, getNumPoints(), firstColumn, columnCount, values);
}

void PointData::setRange(const std::uint64_t firstRow, const std::uint64_t rowCount, const std::uint64_t firstColumn, const std::uint64_t columnCount, const PointDataAccessView& values)
{
    const auto dataLock = lockData();
    const auto numPoints     = getNumPoints();
    const auto numDimensions = getNumDimensions();

    if (firstRow > numPoints || rowCount > numPoints - firstRow || firstColumn > numDimensions || columnCount > numDimensions - firstColumn)
        throw std::out_of_range("Point-data range is out of bounds");

    if (!_storageBackend)
        throw std::logic_error("Point-data storage backend is unavailable");

    const auto elementSize = _storageBackend->getElementSize();

    if (values.rows != rowCount || values.columns != columnCount || values.elementTypeIndex != _storageBackend->getElementTypeIndex() || values.elementSizeBytes != elementSize)
        throw std::invalid_argument("Point-data range view shape or element type does not match the destination");

    if (elementSize == 0 || columnCount > std::numeric_limits<std::uint64_t>::max() / elementSize || values.rowStrideBytes < columnCount * elementSize || (rowCount != 0 && columnCount != 0 && values.data == nullptr))
        throw std::invalid_argument("Point-data range view has an invalid layout");

    _storageBackend->setRange(firstRow, rowCount, firstColumn, columnCount, values);
}

void PointData::materializeStorageBackend(pointdata::storage::StorageBackendProgressCallback progress)
{
    if (!_storageBackend || _storageBackend->getType() == pointdata::storage::StorageBackendType::Dense)
    {
        if (progress)
            progress(1.0f);
        return;
    }

    const auto* sourceBackend = _storageBackend.get();
    const auto rows        = sourceBackend->getNumPoints();
    const auto columns     = sourceBackend->getNumDimensions();
    const auto elementSize = sourceBackend->getElementSize();
    const auto elementTypeIndex = sourceBackend->getElementTypeIndex();
    if (getElementTypeSize(elementTypeIndex) != elementSize)
        throw std::invalid_argument("Source storage backend element size does not match its element type");
    if (rows != 0 && columns > std::numeric_limits<std::uint64_t>::max() / rows)
        throw std::overflow_error("Sparse matrix dimensions overflow the dense element count");

    const auto elementCount = rows * columns;
    if (elementSize == 0 || elementCount > std::numeric_limits<std::size_t>::max() / elementSize)
        throw std::overflow_error("Point-data matrix is too large to materialize");

    VariantOfVectors materializedData;
    setIndexOfVariant(materializedData, elementTypeIndex);
    std::visit([elementCount](auto& vector) { vector.resize(static_cast<std::size_t>(elementCount)); }, materializedData);
    auto* destination = static_cast<char*>(std::visit([](auto& vector) { return static_cast<void*>(vector.data()); }, materializedData));
    sourceBackend->materialize(destination, progress);

    _variantOfVectors = std::move(materializedData);
    _numRows = rows;
    _numDimensions = columns;
    _isDense = true;
    clearStorageBackend();
    refreshStorageBackend();

    if (progress)
        progress(1.0f);
}

const std::vector<QString>& PointData::getDimensionNames() const
{
    const auto dataLock = lockData();
    return _dimNames;
}

void PointData::setData(const std::nullptr_t, const std::size_t numPoints, const std::size_t numDimensions)
{
    const auto dataLock = lockData();

    clearStorageBackend();
    resizeVector(numPoints * numDimensions);
    _numDimensions = static_cast<std::uint64_t>(numDimensions);
    refreshStorageBackend();
}

void PointData::setDimensionNames(const std::vector<QString>& dimNames)
{
    const auto dataLock = lockData();

    if (dimNames.empty())
        return;

    _dimNames = dimNames;

    if (dimNames.size() != _numDimensions)
        qWarning() << "PointData: Number of dimension names does not equal the number of data dimensions";
}

float PointData::getDenseValueAt(const std::size_t index) const
{
    // Intentionally not locking here.
    //
    // This is a fine-grained legacy accessor and is frequently used from
    // parallel loops. Callers must ensure that the underlying point-data
    // storage is not modified concurrently.
    //
    // Use lockData() / the bulk-access APIs when synchronized access is
    // required.

    return std::visit([index](const auto& vec) {
        return static_cast<float>(vec[index]);
    }, _variantOfVectors);
}

float PointData::getValueAt(const std::size_t index) const
{
    return _storageBackend ? _storageBackend->getValueAt(index) : 0.0f;
}

void PointData::setValueAt(const std::size_t index, const float newValue)
{
    // Intentionally not locking here.
    //
    // This is a fine-grained legacy setter and is frequently used from
    // parallel loops. Callers must ensure that the underlying point-data
    // storage is not modified concurrently.
    //
    // Use lockData() / the bulk-access APIs when synchronized access is
    // required.

    materializeStorageBackend();

    std::visit([index, newValue](auto& vec) {
        using value_type = typename std::remove_reference_t<decltype(vec)>::value_type;
        vec[index] = static_cast<value_type>(newValue);
    }, _variantOfVectors);
}

UniqueWorkflowPlan PointData::fromVariantMapWorkflow(QVariantMap variantMap)
{
    //Plugin::fromVariantMap(variantMap);

    const auto appVersion = mv::projects().getCurrentProject()->getApplicationVersionAction().getVersion();

    UniqueWorkflowPlan plan = std::make_unique<WorkflowPlan>(QString("%1(%2)").arg(getSerializationName()).arg(__FUNCTION__));

    if (appVersion < Version(1, 5, 0)) {
        plan->addSequentialStage("Load legacy point data (< 1.5.0)", [this, variantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) {
            const auto dataLock = lockData();
            legacy::PointDataLegacySerializer::fromVariantMapPre150(*this, variantMap, executionContext);
        });

        return plan;
    }

    plan->addSequentialStage("Allocate and populate storage", [this, variantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) {
        const auto dataLock = lockData();

        variantMapMustContain(variantMap, "Data");
        variantMapMustContain(variantMap, "NumberOfPoints");        // not used
        variantMapMustContain(variantMap, "NumberOfDimensions");

        const auto dataMap  = variantMap["Data"].toMap();
        if (dataMap.isEmpty())
            return;

        _isDense            = variantMap.value("Dense", true).toBool();
        _numDimensions      = static_cast<std::size_t>(variantMap["NumberOfDimensions"].toULongLong());
        if (dataMap.contains("ProjectOpenStagingPolicy"))
            setProjectResourceStagingPolicy(mv::util::projectResourceStagingPolicyFromName(dataMap.value("ProjectOpenStagingPolicy").toString()));
        else if (dataMap.contains("Tiled") || dataMap.contains("Sparse") || dataMap.contains("Raw"))
            setProjectResourceStagingPolicy(mv::util::ProjectResourceStagingPolicy::Inherit);
        clearStorageBackend();

        if (dataMap.contains("Tiled") && dataMap["Tiled"].canConvert<QVariantMap>())
        {
            const auto elementType = dataMap.contains("TypeName")
                ? elementTypeSpecifier(dataMap.value("TypeName").toString())
                : static_cast<ElementTypeSpecifier>(dataMap.value("TypeIndex").toInt());

            setElementTypeSpecifier(elementType);
            _numRows = variantMap["NumberOfPoints"].toULongLong();
            const auto elementSize = std::visit([](const auto& vector) { return sizeof(typename std::decay_t<decltype(vector)>::value_type); }, _variantOfVectors);
            const auto tiledStorage = mv::pointdata::tiled::PointDataTiledSerialization::fromVariantMap(dataMap["Tiled"].toMap(), _numRows, _numDimensions, elementSize);
            _storageBackend = std::make_unique<pointdata::storage::TiledPointsStorageBackend>(tiledStorage, static_cast<std::uint32_t>(elementType));
        }
        else if (dataMap.contains("Sparse") && dataMap["Sparse"].canConvert<QVariantMap>())
        {
            const auto sparseMap = dataMap["Sparse"].toMap();
            variantMapMustContain(sparseMap, "Layout");
            variantMapMustContain(sparseMap, "Pointers");
            variantMapMustContain(sparseMap, "Indices");
            variantMapMustContain(sparseMap, "Values");

            const auto elementType = dataMap.contains("TypeName")
                ? elementTypeSpecifier(dataMap.value("TypeName").toString())
                : static_cast<ElementTypeSpecifier>(dataMap.value("TypeIndex").toInt());
            setElementTypeSpecifier(elementType);
            resizeVector(0);
            _numRows = 0;

            auto pointerBlob = sparseMap["Pointers"].toMap();
            auto indexBlob   = sparseMap["Indices"].toMap();
            auto valueBlob   = sparseMap["Values"].toMap();
            const auto pointerByteSize = pointerBlob.value("Size").toULongLong();
            const auto indexByteSize = indexBlob.value("Size").toULongLong();
            const auto valueByteSize = valueBlob.value("Size").toULongLong();
            const auto elementSize = elementType == ElementTypeSpecifier::bfloat16 || elementType == ElementTypeSpecifier::int16 || elementType == ElementTypeSpecifier::uint16 ? 2 :
                elementType == ElementTypeSpecifier::int8 || elementType == ElementTypeSpecifier::uint8 ? 1 : 4;
            const auto layout = sparseMap["Layout"].toInt();
            if (pointerByteSize % sizeof(std::uint64_t) != 0 || indexByteSize % sizeof(std::uint64_t) != 0 || valueByteSize % elementSize != 0 ||
                pointerByteSize > std::numeric_limits<std::size_t>::max() || indexByteSize > std::numeric_limits<std::size_t>::max() || valueByteSize > std::numeric_limits<std::size_t>::max() ||
                (layout != static_cast<int>(pointdata::storage::SparseMatrixLayout::RowCompressed) && layout != static_cast<int>(pointdata::storage::SparseMatrixLayout::ColumnCompressed)))
                throw std::invalid_argument("Serialized sparse point-data arrays have invalid byte sizes");

            std::vector<std::uint64_t> pointers(static_cast<std::size_t>(pointerByteSize / sizeof(std::uint64_t)));
            std::vector<std::uint64_t> indices(static_cast<std::size_t>(indexByteSize / sizeof(std::uint64_t)));
            std::vector<std::uint8_t> values(static_cast<std::size_t>(valueByteSize));
            if (pointerByteSize > 0)
                populateBytesFromBlobMap(pointerBlob, reinterpret_cast<char*>(pointers.data()), pointerByteSize);
            if (indexByteSize > 0)
                populateBytesFromBlobMap(indexBlob, reinterpret_cast<char*>(indices.data()), indexByteSize);
            if (valueByteSize > 0)
                populateBytesFromBlobMap(valueBlob, reinterpret_cast<char*>(values.data()), valueByteSize);

            const auto rows = variantMap["NumberOfPoints"].toULongLong();
            const auto sparseLayout = layout == static_cast<int>(pointdata::storage::SparseMatrixLayout::ColumnCompressed)
                ? pointdata::storage::SparseMatrixLayout::ColumnCompressed
                : pointdata::storage::SparseMatrixLayout::RowCompressed;
            auto sparseBackend = std::make_unique<pointdata::storage::SparsePointsStorageBackend>(rows, _numDimensions, sparseLayout, static_cast<std::uint32_t>(elementType), elementSize, std::move(pointers), std::move(indices), std::move(values));
            initializeStorageBackend(std::move(sparseBackend));
        }
        else if (dataMap.contains("Raw") && dataMap["Raw"].canConvert<QVariantMap>())
        {
            variantMapMustContain(dataMap, "NumberOfElements");

            const auto numberOfElements = static_cast<std::size_t>(dataMap["NumberOfElements"].toULongLong());

            const auto elementTypeIndex = dataMap.contains("TypeName")
                ? elementTypeSpecifier(dataMap.value("TypeName").toString())
                : static_cast<ElementTypeSpecifier>(dataMap.value("TypeIndex").toInt());

            setElementTypeSpecifier(elementTypeIndex);
            resizeVector(numberOfElements);

            // Keep allocation and all writes to the exposed buffer mutually
            // exclusive. The synchronous decoder is intentional here: a
            // std::mutex lock may not be handed from this workflow worker to
            // the parallel workers used by populateBytesFromBlobMapWorkflow().
            const auto rawDataSize = getRawDataSize();

            if (rawDataSize > 0)
                populateBytesFromBlobMap(dataMap["Raw"].toMap(), static_cast<char*>(getDataVoidPtr()), rawDataSize);

            refreshStorageBackend();
        }
    });

    return plan;
}

UniqueWorkflowPlan PointData::toVariantMapWorkflow() const
{
    auto plan = std::make_unique<WorkflowPlan>(__FUNCTION__);
    bool isDense;
    bool isTiled;
    bool isSparse;
    std::shared_ptr<mv::pointdata::tiled::PointDataTiledStorage> tiledStorage;
    mv::util::ProjectResourceStagingPolicy projectOpenStagingPolicy = mv::util::ProjectResourceStagingPolicy::Inherit;

    {
        const auto dataLock = lockData();
        isDense = _isDense;
        projectOpenStagingPolicy = getProjectResourceStagingPolicy();
        if (const auto* tiledBackend = dynamic_cast<const pointdata::storage::TiledPointsStorageBackend*>(_storageBackend.get()))
            tiledStorage = tiledBackend->getStorage();
        isTiled = static_cast<bool>(tiledStorage);
        isSparse = _storageBackend && _storageBackend->getType() == pointdata::storage::StorageBackendType::Sparse;
    }

    //const auto baseSaveStage = plan->addNestedWorkflowStage("Save raw data base", [this](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) -> UniqueWorkflowPlan {
    //    return this->Plugin::toVariantMapWorkflow();
    //})
    //
    //
    //;

    if (isTiled)
    {
        const auto tiledDataWeight = std::max(1.0, static_cast<double>(tiledStorage->memoryEstimate().rawDataBytes));
        const auto serializeTiledStage = plan->addNestedWorkflowStage("Serialize tiled point data", [tiledStorage](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) {
            return mv::pointdata::tiled::PointDataTiledSerialization::toVariantMapWorkflow(*tiledStorage);
        }, WorkflowPlan::JobThreadAffinity::CurrentWorkerThread, tiledDataWeight);

        plan->addSequentialStage("Build tiled point data map", [this, serializeTiledStage, projectOpenStagingPolicy](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) {
            const auto dataLock = lockData();
            QVariantMap outputMap;

            outputMap["TypeIndex"]        = static_cast<std::int32_t>(getElementTypeSpecifier());
            outputMap["TypeName"]         = elementTypeName(getElementTypeSpecifier());
            outputMap["Tiled"]            = executionContext->takeOutput(serializeTiledStage).toMap();
            outputMap["ProjectOpenStagingPolicy"] = mv::util::projectResourceStagingPolicyName(projectOpenStagingPolicy);
            outputMap["NumberOfElements"] = QVariant::fromValue(static_cast<qulonglong>(getNumberOfElements()));

            executionContext->setOutput(outputMap);
        });
    }
    else if (isSparse)
    {
        plan->addSequentialStage("Serialize sparse point data", [this, projectOpenStagingPolicy](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) {
            const auto dataLock = lockData();
            const auto* sparseStorage = dynamic_cast<const pointdata::storage::SparsePointsStorageBackend*>(_storageBackend.get());
            if (!sparseStorage)
                throw std::logic_error("Sparse point-data storage is unavailable");

            const auto& pointers = sparseStorage->getPointers();
            const auto& indices = sparseStorage->getIndices();
            const auto& values = sparseStorage->getValues();
            QVariantMap sparseMap;
            sparseMap["Layout"] = static_cast<std::int32_t>(sparseStorage->getLayout());
            sparseMap["Pointers"] = bytesToBlobVariantMap(reinterpret_cast<const char*>(pointers.data()), pointers.size() * sizeof(std::uint64_t));
            sparseMap["Indices"] = bytesToBlobVariantMap(reinterpret_cast<const char*>(indices.data()), indices.size() * sizeof(std::uint64_t));
            sparseMap["Values"] = bytesToBlobVariantMap(reinterpret_cast<const char*>(values.data()), values.size());

            QVariantMap outputMap;
            outputMap["TypeIndex"] = static_cast<std::int32_t>(getElementTypeSpecifier());
            outputMap["TypeName"] = elementTypeName(getElementTypeSpecifier());
            outputMap["Sparse"] = sparseMap;
            outputMap["ProjectOpenStagingPolicy"] = mv::util::projectResourceStagingPolicyName(projectOpenStagingPolicy);
            outputMap["NumberOfElements"] = QVariant::fromValue(static_cast<qulonglong>(getNumberOfElements()));
            executionContext->setOutput(outputMap);
        });
    }
    else if (isDense)
    {
        const auto storeRawStage = plan->addNestedWorkflowStage("Save raw", [this](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) {
            const auto dataLock = lockData();
            const auto rawMap   = bytesToBlobVariantMap(static_cast<const char*>(getDataConstVoidPtr()), getRawDataSize());
            auto storePlan      = std::make_unique<WorkflowPlan>("Publish point data blob");

            storePlan->addSequentialStage("Publish point data blob", [rawMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& storeExecutionContext) {
                storeExecutionContext->setOutput(rawMap);
            });

            return storePlan;
        });

        plan->addSequentialStage("Build map", [this, storeRawStage, projectOpenStagingPolicy](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) {
            const auto dataLock = lockData();
            QVariantMap outputMap;

            const auto typeSpecifier        = getElementTypeSpecifier();
            const auto typeSpecifierName    = getElementTypeNames()[static_cast<std::int32_t>(typeSpecifier)];
            const auto typeIndex            = static_cast<std::int32_t>(typeSpecifier);
            const auto rawMap               = executionContext->takeOutput(storeRawStage).toMap();

            outputMap.insert("TypeIndex", QVariant::fromValue(typeIndex));
            outputMap.insert("TypeName", QVariant(typeSpecifierName));
            outputMap.insert("Raw", rawMap);
            outputMap.insert("ProjectOpenStagingPolicy", mv::util::projectResourceStagingPolicyName(projectOpenStagingPolicy));
            outputMap.insert("NumberOfElements", QVariant::fromValue(getNumberOfElements()));

            const auto expectedBytes = getRawDataSize();
            const auto blobTotalSize = rawMap["Size"].toULongLong();

            if (blobTotalSize != expectedBytes)
            {
                throw ManiVaultException(
                    SeverityLevel::Error,
                    "PointData blob size does not match declared point-data dimensions",
                    QString("Dataset '%1' declares %2 points x %3 dimensions = %4 bytes, but blob total size is %5 bytes")
                    .arg(getGuiName())
                    .arg(getNumPoints())
                    .arg(getNumDimensions())
                    .arg(expectedBytes)
                    .arg(blobTotalSize),
                    __FUNCTION__
                );
            }

            executionContext->setOutput(outputMap);
        });
    }
    else
    {
        plan->addSequentialStage("Build map", [this, projectOpenStagingPolicy](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) {
            const auto dataLock = lockData();
            QVariantMap outputMap;

            std::vector<char> bytes;

            const auto& indexPointers   = _sparseData.getIndexPointers();
            const auto& colIndices      = _sparseData.getColIndices();
            const auto& values          = _sparseData.getValues();

            const auto* indexPointersBytes  = reinterpret_cast<const char*>(indexPointers.data());
            const auto* colIndicesBytes     = reinterpret_cast<const char*>(colIndices.data());
            const auto* valuesBytes         = reinterpret_cast<const char*>(values.data());

            bytes.insert(bytes.end(), indexPointersBytes, indexPointersBytes + indexPointers.size() * sizeof(size_t));
            bytes.insert(bytes.end(), colIndicesBytes, colIndicesBytes + colIndices.size() * sizeof(size_t));
            bytes.insert(bytes.end(), valuesBytes, valuesBytes + values.size() * sizeof(float));

            outputMap.insert("Raw", bytesToBlobVariantMap(bytes.data(), bytes.size()));
            outputMap.insert("ProjectOpenStagingPolicy", mv::util::projectResourceStagingPolicyName(projectOpenStagingPolicy));

            executionContext->setOutput(outputMap);
        });
    }

    return plan;
}

void PointData::extractFullDataForDimension(std::vector<float>& result, const int dimensionIndex) const
{
    const auto dataLock = lockData();

    CheckDimensionIndex(dimensionIndex);

    result.resize(getNumPoints());

    if (_storageBackend && _storageBackend->getType() != pointdata::storage::StorageBackendType::Dense)
    {
        for (std::size_t i = 0; i < result.size(); ++i)
            result[i] = getValueAt(i * _numDimensions + static_cast<std::size_t>(dimensionIndex));
        return;
    }

    std::visit(
        [&result, this, dimensionIndex](const auto& vec)
        {
            const auto resultSize = result.size();

            for (std::size_t i{}; i < resultSize; ++i)
                result[i] = vec[i * _numDimensions + dimensionIndex];

        },
        _variantOfVectors);
}


void PointData::extractFullDataForDimensions(std::vector<mv::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2) const
{
    const auto dataLock = lockData();

    if (_isDense)
    {
        CheckDimensionIndex(dimensionIndex1);
        CheckDimensionIndex(dimensionIndex2);

        result.resize(getNumPoints());

        if (_storageBackend && _storageBackend->getType() != pointdata::storage::StorageBackendType::Dense)
        {
            for (std::size_t i = 0; i < result.size(); ++i)
            {
                const auto n = i * _numDimensions;
                result[i].set(getValueAt(n + dimensionIndex1), getValueAt(n + dimensionIndex2));
            }
            return;
        }

        std::visit(
            [&result, this, dimensionIndex1, dimensionIndex2](const auto& vec)
            {
                const auto resultSize = result.size();

                for (std::size_t i{}; i < resultSize; ++i)
                {
                    const auto n = i * _numDimensions;
                    result[i].set(vec[n + dimensionIndex1], vec[n + dimensionIndex2]);
                }
            },
            _variantOfVectors);
    }
    else
    {
        result.resize(getNumPoints());

        std::vector<float> col1 = _sparseData.getDenseCol(dimensionIndex1);
        std::vector<float> col2 = _sparseData.getDenseCol(dimensionIndex2);

        for (size_t i = 0; i < result.size(); i++)
            result[i].set(col1[i], col2[i]);
    }
}

void PointData::extractDataForDimension(std::vector<float>& result, const int dimensionIndex, const std::vector<std::uint32_t>& indices) const
{
    CheckDimensionIndex(dimensionIndex);

    result.resize(indices.size());

    if (_storageBackend && _storageBackend->getType() != pointdata::storage::StorageBackendType::Dense)
    {
        for (std::size_t i = 0; i < result.size(); ++i)
            result[i] = getValueAt(static_cast<std::size_t>(indices[i]) * _numDimensions + static_cast<std::size_t>(dimensionIndex));
        return;
    }

    std::visit([&result, this, dimensionIndex, indices](const auto& vec)
        {
            const auto resultSize = result.size();

            for (std::size_t i{}; i < resultSize; ++i)
            {
                const auto n = std::size_t{ indices[i] } * _numDimensions;
                result[i] = vec[n + dimensionIndex];
            }
        },
        _variantOfVectors);
}

void PointData::extractDataForDimensions(std::vector<mv::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2, const std::vector<std::uint32_t>& indices) const
{
    const auto dataLock = lockData();

    CheckDimensionIndex(dimensionIndex1);
    CheckDimensionIndex(dimensionIndex2);

    result.resize(indices.size());

    if (_storageBackend && _storageBackend->getType() != pointdata::storage::StorageBackendType::Dense)
    {
        for (std::size_t i = 0; i < result.size(); ++i)
        {
            const auto n = static_cast<std::size_t>(indices[i]) * _numDimensions;
            result[i].set(getValueAt(n + dimensionIndex1), getValueAt(n + dimensionIndex2));
        }
        return;
    }

    std::visit(
        [&result, this, dimensionIndex1, dimensionIndex2, indices](const auto& vec)
        {
            const auto resultSize = result.size();

            for (std::size_t i{}; i < resultSize; ++i)
            {
                const auto n = std::size_t{ indices[i] } *_numDimensions;
                result[i].set(vec[n + dimensionIndex1], vec[n + dimensionIndex2]);
            }
        },
        _variantOfVectors);
}

Points::Points(QString dataName, bool mayUnderive /*= true*/, const QString& guid /*= ""*/) :
    DatasetImpl(dataName, mayUnderive, guid),
    _infoAction(nullptr),
    _pointsAction(nullptr),
    _dimensionsPickerGroupAction(nullptr),
    _dimensionsPickerAction(nullptr)
{
}

void Points::init()
{
    DatasetImpl::init();

    _infoAction = new InfoAction(this, *this);
    _pointsAction = new PointsAction(this, getSmartPointer());

    const auto updateDimensionsPickerAction = [this]() -> void {
        _dimensionsPickerAction->setPointsDataset(*this);
    };

    //if (isFull()) {
        _dimensionsPickerGroupAction = new gui::GroupAction(this, "Dimensions Group");

        _dimensionsPickerGroupAction->setText("Dimensions");
        _dimensionsPickerGroupAction->setShowLabels(false);

        _dimensionsPickerAction = new DimensionsPickerAction(_dimensionsPickerGroupAction, "Dimensions");

        _dimensionsPickerGroupAction->addAction(_dimensionsPickerAction);

        updateDimensionsPickerAction();

        connect(&getSmartPointer(), &Dataset<Points>::dataDimensionsChanged, this, updateDimensionsPickerAction);
    //}


    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetDataSelectionChanged));
    _eventListener.registerDataEventByType(PointType, [this](DatasetEvent* dataEvent)
    {
        switch (dataEvent->getType())
        {
            case EventType::DatasetDataSelectionChanged:
            {
                // Do not process our own selection changes
                if (dataEvent->getDataset() == Dataset<Points>(this))
                    return;

                // Only synchronize when dataset grouping is enabled and our own group index is non-negative
                if (!mv::data().getSelectionGroupingAction()->isChecked() || getGroupIndex() < 0)
                    return;

                // Only synchronize of the group indexes match
                if (dataEvent->getDataset()->getGroupIndex() != getGroupIndex())
                    return;

                // Get smart pointer to foreign points dataset
                auto foreignPoints = dataEvent->getDataset<Points>();

                // Only synchronize when the number of points matches
                if (foreignPoints->getNumPoints() != getNumPoints())
                    return;

                // Get source target indices
                auto& sourceIndices = foreignPoints->getSelection<Points>()->indices;
                auto& targetIndices = getSelection<Points>()->indices;

                // Do nothing if the indices have not changed
                if (sourceIndices == targetIndices)
                    return;

                // Copy indices from source to target if the indices have changed
                targetIndices = sourceIndices;

                events().notifyDatasetDataSelectionChanged(this);

                break;
            }
        }
    });

    setIconByName("database");
}

// =============================================================================
// Point Set
// =============================================================================

void Points::handleNumberDimensionsChanged(std::size_t newNumDimensions) {
    std::vector<QString> dimensionNames = getDimensionNames();
    std::size_t oldDimSize = dimensionNames.size();
    dimensionNames.resize(newNumDimensions, "");

    for (std::size_t addedDimension = oldDimSize; addedDimension < newNumDimensions; addedDimension++) {
        dimensionNames[addedDimension] = QString("Dim %1").arg(addedDimension + 1);
    }

    setDimensionNames(dimensionNames);  // calls notifyDatasetDataDimensionsChanged
}

void Points::setData(std::nullptr_t, const std::size_t numPoints, const std::size_t numDimensions)
{
    const auto numDimensionsChanged = numDimensions != getRawData<PointData>()->getNumDimensions();

    getRawData<PointData>()->setData(nullptr, numPoints, numDimensions);

    if (numDimensionsChanged)
        handleNumberDimensionsChanged(numDimensions);
}

void Points::extractDataForDimension(std::vector<float>& result, const int dimensionIndex) const
{
    if (isProxy()) {
        result.resize(getNumPoints());

        auto pointIndexOffset = 0u;

        for (auto proxyMember : getProxyMembers()) {
            auto points = mv::Dataset<Points>(proxyMember);

            std::vector<float> proxyPoints;

            proxyPoints.resize(points->getNumPoints());

            points->extractDataForDimension(proxyPoints, dimensionIndex);

            std::copy(proxyPoints.begin(), proxyPoints.end(), result.begin() + pointIndexOffset);

            pointIndexOffset += points->getNumPoints();
        }
    }
    else {
        const auto rawPointData = getRawData<PointData>();

        if (isFull())
            rawPointData->extractFullDataForDimension(result, dimensionIndex);
        else
            rawPointData->extractDataForDimension(result, dimensionIndex, indices);

    }
}

void Points::extractDataForDimensions(std::vector<mv::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2) const
{
    if (isProxy()) {
        result.resize(getNumPoints());

        auto pointIndexOffset = 0u;

        for (auto proxyMember : getProxyMembers()) {
            auto points = mv::Dataset<Points>(proxyMember);

            std::vector<mv::Vector2f> proxyPoints;

            proxyPoints.resize(points->getNumPoints());

            points->extractDataForDimensions(proxyPoints, dimensionIndex1, dimensionIndex2);

            std::copy(proxyPoints.begin(), proxyPoints.end(), result.begin() + pointIndexOffset);

            pointIndexOffset += points->getNumPoints();
        }
    }
    else {
        const auto rawPointData = getRawData<PointData>();

        if (isFull())
            rawPointData->extractFullDataForDimensions(result, dimensionIndex1, dimensionIndex2);
        else
            rawPointData->extractDataForDimensions(result, dimensionIndex1, dimensionIndex2, indices);
    }
}

bool Points::mayProxy(const Datasets& proxyDatasets) const
{
    if (!DatasetImpl::mayProxy(proxyDatasets))
        return false;

    QSet<std::int32_t> numberOfDimensionsSet;

    for (auto proxyDataset : proxyDatasets)
        numberOfDimensionsSet.insert(Dataset<Points>(proxyDataset)->getNumDimensions());
    
    if (numberOfDimensionsSet.count() > 1)
        return false;

    return true;
}

Dataset<DatasetImpl> Points::copy() const
{
    auto set = new Points(getRawDataName());

    set->setText(text());
    set->indices = indices;

    return set;
}

Dataset<DatasetImpl> Points::createSubsetFromSelection(const QString& guiName, const Dataset<DatasetImpl>& parentDataSet /*= Dataset<DatasetImpl>()*/, const bool& visible /*= true*/) const
{
    return mv::data().createSubsetFromSelection(getSelection(), toSmartPointer(), guiName, parentDataSet, visible);
}

Dataset<DatasetImpl> Points::createSubsetFromVisibleSelection(const QString& guiName, const Dataset<DatasetImpl>& parentDataSet /*= Dataset<DatasetImpl>()*/, const bool& visible /*= true*/) const
{
    Dataset<Points> subsetSelection = getSelection()->copy();
    std::vector<uint32_t>& selectionIndices = subsetSelection->getSelectionIndices();

    //// Get the global indices of the parent dataset
    //std::vector<uint32_t> globalIndices;
    //getGlobalIndices(globalIndices);

    //// Find the intersection of points that are selected, and the given indices
    //std::vector<uint32_t>& smallVector = (globalIndices.size() < selectionIndices.size()) ? globalIndices : selectionIndices;
    //std::vector<uint32_t>& largeVector = (globalIndices.size() < selectionIndices.size()) ? selectionIndices : globalIndices;
    //
    //// Sort the smallest vector
    //sort(smallVector.begin(), smallVector.end());

    //// Loop over large vector and binary search which values are in the smaller vector
    //std::vector<uint32_t> intersection;
    //std::copy_if(largeVector.begin(), largeVector.end(), std::back_inserter(intersection), [smallVector](uint32_t x)
    //{
    //    return std::binary_search(smallVector.begin(), smallVector.end(), x);
    //});

    // Compute the indices that are selected in this local dataset
    std::vector<uint32_t> localSelectionIndices;
    getLocalSelectionIndices(localSelectionIndices);

    // If we make a subset from a subset, take the locally selected points from the previous subset
    if (!isFull())
    {
        for (uint32_t& localIndex : localSelectionIndices)
            localIndex = indices[localIndex];
    }
    // If the data is full, then the locally selected points are the new subset

    subsetSelection->indices = localSelectionIndices;

    return mv::data().createSubsetFromSelection(subsetSelection, toSmartPointer(), guiName, parentDataSet, visible);
}

void Points::setProxyMembers(const Datasets& proxyMembers)
{
    DatasetImpl::setProxyMembers(proxyMembers);

    getTask().setName("Creating proxy");
    getTask().setVisible(!(projects().isOpeningProject() || projects().isImportingProject()));
    getTask().setRunning();

    std::uint64_t pointIndexOffset = 0lu;

    QCoreApplication::processEvents();

    for (auto proxyMember : getProxyMembers()) {
        auto targetPoints = Dataset<Points>(proxyMember);

        std::vector<std::uint32_t> targetGlobalIndices;

        targetPoints->getGlobalIndices(targetGlobalIndices);

        // Selection map from proxy to member
        {
            SelectionMap selectionMapToTarget;

            for (std::uint64_t pointIndex = 0; pointIndex < targetPoints->getNumPoints(); ++pointIndex)
                selectionMapToTarget.getMap()[pointIndexOffset + pointIndex] = std::vector<std::uint32_t>({ targetGlobalIndices[pointIndex] });

            addLinkedData(targetPoints, selectionMapToTarget);
        }

        // Selection map from member to proxy
        {
            SelectionMap selectionMapToSource;

            for (std::uint64_t pointIndex = 0; pointIndex < targetPoints->getNumPoints(); ++pointIndex)
                selectionMapToSource.getMap()[targetGlobalIndices[pointIndex]] = std::vector<std::uint32_t>({ ::local::safe_numeric_cast<std::uint32_t>(pointIndexOffset + pointIndex) });

            targetPoints->addLinkedData(toSmartPointer(), selectionMapToSource);

            if (!targetPoints->isFull())
                targetPoints->getFullDataset<Points>()->addLinkedData(toSmartPointer(), selectionMapToSource);
        }

        pointIndexOffset += targetPoints->getNumPoints();

        getTask().setProgressDescription(QString("Creating mappings for %1").arg(proxyMember->text()));
        getTask().setProgress(static_cast<float>(getProxyMembers().indexOf(proxyMember)) / static_cast<float>(getProxyMembers().count()));

        QCoreApplication::processEvents();
    }

    if (!getProxyMembers().isEmpty()) {
        setDimensionNames(Dataset<Points>(getProxyMembers().first())->getDimensionNames());
        events().notifyDatasetDataDimensionsChanged(this);
    }

    getTask().setFinished();
}

/* -------------------------------------------------------------------------- */
/*                            Index transformation                            */
/* -------------------------------------------------------------------------- */

void Points::getGlobalIndices(std::vector<std::uint32_t>& globalIndices) const
{
    if (isProxy())
    {
        globalIndices.resize(getNumPoints(), 0);
        std::iota(globalIndices.begin(), globalIndices.end(), 0);
        return;
    }

    // Traverse the chain of datasets back to the original source data
    // Any subsets traversed along the way are stored in the a subset chain
    std::vector<Dataset<Points>> subsetChain;
    {
        auto currentDataset = toSmartPointer<Points>();

        // Walk back in the chain of derived data until we find the original source
        while (currentDataset->isDerivedData())
        {
            // If the current set is a subset then store it on the stack to traverse later
            if (!currentDataset->isFull())
                subsetChain.push_back(currentDataset);

            currentDataset = currentDataset->getNextSourceDataset<Points>();
        }

        // We now have a non-derived dataset bound, push it if its also a subset
        if (!currentDataset->isFull())
            subsetChain.push_back(currentDataset);
    }

    // Find the original global indices of this dataset by transforming them
    // step by step traversing through the chain of subsets
    {
        globalIndices.resize(getNumPoints(), 0);
        std::iota(globalIndices.begin(), globalIndices.end(), 0);

        for (const Dataset<Points>& subset : subsetChain)
        {
            for (std::uint64_t i = 0; i < globalIndices.size(); i++)
                globalIndices[i] = subset->indices[globalIndices[i]];
        }
    }
}

void Points::selectedLocalIndices(const std::vector<std::uint32_t>& selectionIndices, std::vector<bool>& selected) const
{
    //Timer timer(__FUNCTION__);

    // Find the global indices of this dataset
    std::vector<std::uint32_t> localGlobalIndices;
    getGlobalIndices(localGlobalIndices);

    if (isProxy()) {
        selected.resize(getNumPoints(), false);

        std::size_t invalidSelectionIndices = 0;
        for (const auto& selectionIndex : selectionIndices) {
            if (selectionIndex >= localGlobalIndices.size()) {
                ++invalidSelectionIndices;
                continue;
            }

            const auto localIndex = localGlobalIndices[selectionIndex];
            if (localIndex >= selected.size()) {
                ++invalidSelectionIndices;
                continue;
            }

            selected[localIndex] = true;
        }

        if (invalidSelectionIndices > 0)
            qWarning() << "Ignoring" << invalidSelectionIndices << "out-of-range selection indices for proxy point dataset" << getId()
                       << "(point count:" << getNumPoints() << ")";
    }
    else {
        // In an array the size of the full raw data, mark selected points as true
        const auto sourceDataset = getSourceDataset<Points>();
        const auto rawPointCount = sourceDataset->getNumRawPoints();
        std::vector<bool> globalSelection(rawPointCount, false);

        std::size_t invalidSelectionIndices = 0;
        for (const std::uint32_t& selectionIndex : selectionIndices) {
            if (selectionIndex >= rawPointCount) {
                ++invalidSelectionIndices;
                continue;
            }

            globalSelection[selectionIndex] = true;
        }

        std::size_t invalidGlobalIndices = 0;

        // For all local points find out which are selected
        selected.resize(localGlobalIndices.size(), false);
        for (std::uint64_t i = 0; i < localGlobalIndices.size(); i++)
        {
            if (localGlobalIndices[i] >= rawPointCount) {
                ++invalidGlobalIndices;
                continue;
            }

            if (globalSelection[localGlobalIndices[i]])
                selected[i] = true;
        }

        if (invalidSelectionIndices > 0 || invalidGlobalIndices > 0)
            qWarning() << "Ignoring invalid point indices while mapping selection. Dataset:" << getId() << getGuiName()
                       << "points:" << getNumPoints() << "full:" << isFull() << "proxy:" << isProxy() << "derived:" << isDerivedData()
                       << "source:" << sourceDataset->getId() << sourceDataset->getGuiName()
                       << "source points:" << sourceDataset->getNumPoints() << "source raw points:" << rawPointCount
                       << "source full:" << sourceDataset->isFull() << "source proxy:" << sourceDataset->isProxy() << "source derived:" << sourceDataset->isDerivedData()
                       << ", selection IDs:" << selectionIndices.size()
                       << ", invalid selection IDs:" << invalidSelectionIndices
                       << ", invalid dataset IDs:" << invalidGlobalIndices << ")";
    }
}

void Points::getLocalSelectionIndices(std::vector<std::uint32_t>& localSelectionIndices) const
{
    if (isProxy())
    {
        localSelectionIndices = getSelection<Points>()->indices;
        return;
    }


    auto selection = getSelection<Points>();

    // Find the global indices of this dataset
    std::vector<std::uint32_t> localGlobalIndices;
    getGlobalIndices(localGlobalIndices);

    // In an array the size of the full raw data, mark selected points as true
    std::vector<bool> globalSelection(getSourceDataset<Points>()->getNumRawPoints(), false);
    for (const std::uint32_t& selectionIndex : selection->indices)
        globalSelection[selectionIndex] = true;

    // For all local points find out which are selected
    std::vector<bool> selected(localGlobalIndices.size(), false);
    int indexCount = 0;
    for (std::uint64_t i = 0; i < localGlobalIndices.size(); i++)
    {
        if (globalSelection[localGlobalIndices[i]])
        {
            selected[i] = true;
            indexCount++;
        }
    }

    localSelectionIndices.resize(indexCount);
    std::uint64_t c = 0;
    for (std::uint64_t i = 0; i < selected.size(); i++)
    {
        if (selected[i])
            localSelectionIndices[c++] = i;
    }
}

/* -------------------------------------------------------------------------- */
/*                               Action getters                               */
/* -------------------------------------------------------------------------- */

InfoAction& Points::getInfoAction()
{
    return *_infoAction;
}

PointsAction& Points::getPointsAction()
{
    return *_pointsAction;
}

mv::gui::GroupAction& Points::getDimensionsPickerGroupAction()
{
    if (!isFull())
        return getFullDataset<Points>()->getDimensionsPickerGroupAction();

    return *_dimensionsPickerGroupAction;
}

DimensionsPickerAction& Points::getDimensionsPickerAction()
{
    if (!isFull())
        return getFullDataset<Points>()->getDimensionsPickerAction();

    return *_dimensionsPickerAction;
}

/* -------------------------------------------------------------------------- */
/*                                 Selection                                  */
/* -------------------------------------------------------------------------- */

std::vector<std::uint32_t>& Points::getSelectionIndices()
{
    return getSelection<Points>()->indices;
}

const std::vector<QString>& Points::getDimensionNames() const
{
    if (isProxy()) {
        return mv::Dataset<Points>(getProxyMembers().first())->getDimensionNames();
    }
    else {
        return getRawData<PointData>()->getDimensionNames();
    }
}

void Points::setDimensionNames(const std::vector<QString>& dimNames)
{
    assert(dimNames.size() == getRawData<PointData>()->getNumDimensions());

    getRawData<PointData>()->setDimensionNames(dimNames);

    mv::events().notifyDatasetDataDimensionsChanged(this);
}

mv::util::ProjectResourceStagingPolicy Points::getProjectResourceStagingPolicy() const
{
    if (isProxy())
        return mv::util::ProjectResourceStagingPolicy::Inherit;

    return getRawData<PointData>()->getProjectResourceStagingPolicy();
}

void Points::setProjectResourceStagingPolicy(const mv::util::ProjectResourceStagingPolicy policy)
{
    if (isProxy())
        throw std::logic_error("Cannot set resource staging policy on proxy points");

    getRawData<PointData>()->setProjectResourceStagingPolicy(policy);
}

mv::util::ProjectResourceStagingPolicy Points::getProjectOpenStagingPolicy() const
{
    return getProjectResourceStagingPolicy();
}

void Points::setProjectOpenStagingPolicy(const mv::util::ProjectResourceStagingPolicy policy)
{
    setProjectResourceStagingPolicy(policy);
}

QString Points::getElementTypeName() const
{
    if (isProxy())
    {
        const auto proxyMembers = getProxyMembers();

        return proxyMembers.isEmpty() ? PointData::elementTypeName(ElementTypeSpecifier::float32) : mv::Dataset<Points>(proxyMembers.first())->getElementTypeName();
    }

    return getRawData<PointData>()->getElementTypeName();
}

std::optional<Points::StorageBackendType> Points::getStorageBackendType() const
{
    if (isProxy())
    {
        const auto proxyMembers = getProxyMembers();

        if (proxyMembers.isEmpty())
            return std::nullopt;

        const auto firstType = mv::Dataset<Points>(proxyMembers.first())->getStorageBackendType();

        if (!firstType)
            return std::nullopt;

        for (const auto& proxyMember : proxyMembers)
        {
            if (mv::Dataset<Points>(proxyMember)->getStorageBackendType() != firstType)
                return std::nullopt;
        }

        return firstType;
    }

    return getRawData<PointData>()->getStorageBackendType();
}

pointdata::storage::StorageBackendCapabilities Points::getStorageBackendCapabilities() const
{
    if (isProxy())
        return 0;

    return getRawData<PointData>()->getStorageBackendCapabilities();
}

const pointdata::storage::PointsStorageBackend* Points::getStorageBackend() const
{
    return isProxy() ? nullptr : getRawData<PointData>()->getStorageBackend();
}

void Points::initializeStorageBackend(std::unique_ptr<pointdata::storage::PointsStorageBackend> storageBackend)
{
    if (isProxy())
        throw std::invalid_argument("Storage backend initialization is only supported for concrete point datasets");

    getRawData<PointData>()->initializeStorageBackend(std::move(storageBackend));
    mv::events().notifyDatasetDataChanged(this);
}

void Points::convertToStorageBackend(const StorageBackendType type, const StorageBackendConfiguration& configuration, StorageBackendProgressCallback progress)
{
    if (isProxy())
        throw std::invalid_argument("Storage backend conversion is only supported for concrete point datasets");

    getRawData<PointData>()->convertToStorageBackend(type, configuration, std::move(progress));
    mv::events().notifyDatasetDataChanged(this);
}

std::unique_ptr<mv::gui::StorageBackendConversionAction> Points::createStorageBackendConversionAction(const StorageBackendType targetType, QObject* parent) const
{
    if (isProxy())
        return nullptr;

    return getRawData<PointData>()->createStorageBackendConversionAction(targetType, parent);
}

void Points::convertToStorageBackend(const StorageBackendType type, const StorageBackendConfigurationOptionsPtr options, StorageBackendProgressCallback progress)
{
    if (isProxy())
        throw std::invalid_argument("Storage backend conversion is only supported for concrete point datasets");

    getRawData<PointData>()->convertToStorageBackend(type, std::move(options), std::move(progress));
    mv::events().notifyDatasetDataChanged(this);
}

void Points::configureStorageBackend(const StorageBackendConfigurationOptionsPtr options)
{
    if (isProxy())
        throw std::invalid_argument("Storage backend configuration is only supported for concrete point datasets");

    getRawData<PointData>()->configureStorageBackend(std::move(options));
    mv::events().notifyDatasetDataChanged(this);
}

PointDataAccessView Points::getRow(const std::uint64_t row) const
{
    if (row >= getNumPoints())
        return {};

    if (isProxy())
    {
        auto rowOffset = row;

        for (const auto& proxyMember : getProxyMembers())
        {
            const auto member = mv::Dataset<Points>(proxyMember);
            const auto memberRowCount = member->getNumPoints();

            if (rowOffset < memberRowCount)
                return member->getRow(rowOffset);

            rowOffset -= memberRowCount;
        }

        return {};
    }

    return isFull() ? getRawData<PointData>()->getRow(row) : getRawData<PointData>()->getRow(indices[static_cast<std::size_t>(row)]);
}

void Points::visitRows(const std::uint64_t firstRow, const std::uint64_t rowCount, const PointDataAccessVisitor& visitor) const
{
    visitRange(firstRow, rowCount, 0, getNumDimensions(), visitor);
}

void Points::visitRows(const std::vector<std::uint64_t>& rowIds, const PointDataAccessVisitor& visitor) const
{
    for (const auto row : rowIds)
        visitRange(row, 1, 0, getNumDimensions(), visitor);
}

void Points::visitColumns(const std::uint64_t firstColumn, const std::uint64_t columnCount, const PointDataAccessVisitor& visitor) const
{
    visitRange(0, getNumPoints(), firstColumn, columnCount, visitor);
}

void Points::visitColumns(const std::vector<std::uint64_t>& columnIds, const PointDataAccessVisitor& visitor) const
{
    for (const auto column : columnIds)
    {
        if (column >= getNumDimensions())
            throw std::out_of_range("Point-data column ID is out of bounds");
    }

    if (isProxy())
    {
        for (const auto& proxyMember : getProxyMembers())
            mv::Dataset<Points>(proxyMember)->visitColumns(columnIds, visitor);

        return;
    }

    const auto* rawPointData = getRawData<PointData>();
    if (isFull())
    {
        rawPointData->visitColumns(columnIds, visitor);
        return;
    }

    std::vector<std::uint64_t> rowIds;
    rowIds.reserve(static_cast<std::size_t>(getNumPoints()));

    for (std::uint64_t row = 0; row < getNumPoints(); ++row)
        rowIds.push_back(indices[static_cast<std::size_t>(row)]);

    rawPointData->visitColumns(columnIds, rowIds, visitor);
}

void Points::visitColumns(const std::uint64_t firstColumn, const std::uint64_t columnCount, const std::vector<std::uint64_t>& rowIds, const PointDataAccessVisitor& visitor) const
{
    for (const auto row : rowIds)
        visitRange(row, 1, firstColumn, columnCount, visitor);
}

void Points::visitRange(const std::uint64_t firstRow, const std::uint64_t rowCount, const std::uint64_t firstColumn, const std::uint64_t columnCount, const PointDataAccessVisitor& visitor) const
{
    if (!visitor || firstRow > getNumPoints() || rowCount > getNumPoints() - firstRow || firstColumn > getNumDimensions() || columnCount > getNumDimensions() - firstColumn)
        throw std::out_of_range("Point-data range is invalid");

    if (rowCount == 0 || columnCount == 0)
        return;

    if (isProxy())
    {
        std::uint64_t proxyRowOffset = 0;
        const auto requestedEnd = firstRow + rowCount;

        for (const auto& proxyMember : getProxyMembers())
        {
            const auto member = mv::Dataset<Points>(proxyMember);
            const auto memberRowCount = member->getNumPoints();
            const auto memberEnd = proxyRowOffset + memberRowCount;
            const auto overlapBegin = std::max(firstRow, proxyRowOffset);
            const auto overlapEnd = std::min(requestedEnd, memberEnd);

            if (overlapBegin < overlapEnd)
                member->visitRange(overlapBegin - proxyRowOffset, overlapEnd - overlapBegin, firstColumn, columnCount, visitor);

            proxyRowOffset = memberEnd;

            if (proxyRowOffset >= requestedEnd)
                break;
        }

        return;
    }

    const auto* rawPointData = getRawData<PointData>();

    if (isFull())
    {
        rawPointData->visitRange(firstRow, rowCount, firstColumn, columnCount, visitor);
        return;
    }

    std::vector<std::uint64_t> rowIds;
    rowIds.reserve(static_cast<std::size_t>(rowCount));

    for (std::uint64_t row = firstRow; row < firstRow + rowCount; ++row)
        rowIds.push_back(indices[static_cast<std::size_t>(row)]);

    rawPointData->visitColumns(firstColumn, columnCount, rowIds, visitor);
}

void Points::setRows(const std::uint64_t firstRow, const std::uint64_t rowCount, const PointDataAccessView& values)
{
    setRange(firstRow, rowCount, 0, getNumDimensions(), values);
}

void Points::setColumns(const std::uint64_t firstColumn, const std::uint64_t columnCount, const PointDataAccessView& values)
{
    setRange(0, getNumPoints(), firstColumn, columnCount, values);
}

void Points::setRange(const std::uint64_t firstRow, const std::uint64_t rowCount, const std::uint64_t firstColumn, const std::uint64_t columnCount, const PointDataAccessView& values)
{
    if (isProxy() || !isFull())
        throw std::logic_error("Point-data range mutation is unavailable for proxy and subset datasets");

    getRawData<PointData>()->setRange(firstRow, rowCount, firstColumn, columnCount, values);
    mv::events().notifyDatasetDataChanged(this);
}

float Points::getValueAt(const std::size_t index) const
{
    return getRawData<PointData>()->getValueAt(index);
}

void Points::setValueAt(const std::size_t index, const float newValue)
{
    getRawData<PointData>()->setValueAt(index, newValue);
}

static void resolveLinkedPointData(const LinkedData& linkedData, const std::vector<std::uint32_t>& indices, Datasets* ignoreDatasets = nullptr)
{
    Dataset<Points> sourceDataset   = linkedData.getSourceDataSet();
    Dataset<Points> targetDataset   = linkedData.getTargetDataset();
    Dataset<Points> targetSelection = targetDataset->getSelection();

    if (sourceDataset->isLocked() || targetDataset->isLocked())
        return;

    Datasets notified;

    if (ignoreDatasets == nullptr)
        ignoreDatasets = &notified;

    // Do not notify if the dataset has already been notified
    if (ignoreDatasets->contains(targetDataset))
        return;

    //qDebug() << QString("%1, %2, %3").arg(__FUNCTION__, sourceDataset->getGuiName(), targetDataset->getGuiName());

    {
        //Timer timer("Creating maps");

        const SelectionMap& mapping = linkedData.getMapping();

        // Create separate vector of additional linked selected points
        std::vector<std::uint32_t> linkedIndices;

        // Reserve at least as much space as required for a 1-1 mapping
        linkedIndices.reserve(indices.size());

        for (const auto& selectionIndex : indices)
        {
            if (mapping.hasMappingForPointIndex(selectionIndex))
            {
                std::vector<std::uint32_t> mappedSelection;
                mapping.populateMappingIndices(selectionIndex, mappedSelection);
                linkedIndices.insert(linkedIndices.end(), mappedSelection.begin(), mappedSelection.end());
            }
        }

        if (targetDataset->isProxy()) {
            std::set<std::uint32_t> targetIndicesSet(targetSelection->indices.begin(), targetSelection->indices.end());

            for (auto& [key, value] : const_cast<SelectionMap&>(mapping).getMap())
                for (const auto& v : value)
                    targetIndicesSet.erase(v);

            for (const auto& linkedIndex : linkedIndices)
                targetIndicesSet.insert(linkedIndex);

            targetSelection->indices = std::vector<std::uint32_t>(targetIndicesSet.begin(), targetIndicesSet.end());
        }
        else {
            targetSelection->indices = linkedIndices;
        }
    }
    
    // Add the target of the linked data (of which we updated the selection indices) to the ignore list
    //*ignoreDatasets << sourceDataset << targetDataset;
    *ignoreDatasets << targetDataset;

    // Recursively resolve linked point data
    for (const mv::LinkedData& targetLd : targetDataset->getLinkedData())
        resolveLinkedPointData(targetLd, targetSelection->indices, ignoreDatasets);
}

void Points::resolveLinkedData(bool force /*= false*/)
{
    if (isLocked())
        return;

    // Check for linked data in this dataset and resolve them
    for (const mv::LinkedData& linkedData : getLinkedData())
        resolveLinkedPointData(linkedData, getSelection<Points>()->indices, nullptr);

    // Check for linked data in all source datasets and resolve them
    // This and all source data share the same selection indices
    mv::Dataset<Points> dataset = Dataset<Points>(this);
    while (dataset->isDerivedData())
    {
        dataset = dataset->getSourceDataset<Points>();

        if (dataset.isValid())
        {
            for (const mv::LinkedData& linkedData : dataset->getLinkedData())
                resolveLinkedPointData(linkedData, getSelection<Points>()->indices, nullptr);

        }
        else
            break;
    }

}

void Points::setSelectionIndices(const std::vector<std::uint32_t>& indices)
{
    //qDebug() << QString("%1, %2").arg(__FUNCTION__, getGuiName());

    if (isLocked())
        return;

    auto selection = getSelection<Points>();

    selection->indices = indices;

    resolveLinkedData();

    //events().notifyDatasetDataSelectionChanged(this);
}

bool Points::canSelect() const
{
    return getNumPoints() > 0;
}

bool Points::canSelectAll() const
{
    return getNumPoints() != getSelectionSize();
}

bool Points::canSelectNone() const
{
    return getSelection<Points>()->indices.size() >= 1;
}

bool Points::canSelectInvert() const
{
    return true;
}

void Points::selectAll()
{
    std::vector<std::uint32_t> selectionIndices;

    selectionIndices.resize(getNumPoints());

    if (isFull())
        std::iota(selectionIndices.begin(), selectionIndices.end(), 0);
    else
        selectionIndices = indices;

    setSelectionIndices(selectionIndices);

    events().notifyDatasetDataSelectionChanged(this);
}

void Points::selectNone()
{
    setSelectionIndices({});

    events().notifyDatasetDataSelectionChanged(this);
}

void Points::selectInvert()
{
    // Get the locally selected indices (the points in the subset that are selected)
    std::vector<std::uint32_t> localSelectionIndices;
    getLocalSelectionIndices(localSelectionIndices);

    // Compute the inverse of this
    const auto numberOfPoints = getNumPoints();
    std::set<std::uint32_t> selectionSet(localSelectionIndices.begin(), localSelectionIndices.end());

    std::vector<std::uint32_t> selectionIndices;
    selectionIndices.reserve(numberOfPoints - selectionSet.size());

    for (std::uint64_t i = 0; i < numberOfPoints; i++)
        if (selectionSet.find(i) == selectionSet.end())
            selectionIndices.push_back(i);

    // Convert the inverted indices back to global indices
    std::vector<std::uint32_t> globalIndices;
    getGlobalIndices(globalIndices);

    for (std::uint32_t& index : selectionIndices)
    {
        index = globalIndices[index];
    }

    setSelectionIndices(selectionIndices);

    events().notifyDatasetDataSelectionChanged(this);
}

UniqueWorkflowPlan Points::fromVariantMapWorkflow(QVariantMap variantMap)
{
    auto plan = std::make_unique<WorkflowPlan>(__FUNCTION__);

    plan->addNestedWorkflowStage("Load dataset base", [this, variantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) -> UniqueWorkflowPlan {
        return this->DatasetImpl::fromVariantMapWorkflow(variantMap);
    });

    const auto appVersion = mv::projects().getCurrentProject()->getApplicationVersionAction().getVersion();
    const auto restoreProjectResourceStagingPolicy = [this, variantMap] {
        const auto dataMap = variantMap.value("Data").toMap();
        if (dataMap.contains("ProjectOpenStagingPolicy"))
            setProjectResourceStagingPolicy(mv::util::projectResourceStagingPolicyFromName(dataMap.value("ProjectOpenStagingPolicy").toString()));
        else if (isFull())
            setProjectResourceStagingPolicy(mv::util::ProjectResourceStagingPolicy::Inherit);

        if (dataMap.contains("ProjectOpenStagingPolicy") || isFull())
            mv::events().notifyDatasetDataChanged(this);
    };

    if (appVersion < Version(1, 5, 0)) {
        plan->addSequentialStage("Load legacy points raw data (< 1.5.0)", [this, variantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) {
            legacy::PointsLegacySerializer::fromVariantMapPre150(*this, variantMap, executionContext);
        }, WorkflowPlan::JobThreadAffinity::GuiThread);

        plan->addSequentialStage("Restore project resource staging policy", [restoreProjectResourceStagingPolicy](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) {
            restoreProjectResourceStagingPolicy();
        }, WorkflowPlan::JobThreadAffinity::GuiThread);

        return plan;
    }

    plan->addNestedWorkflowStage("Load raw data", [this, variantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) mutable -> UniqueWorkflowPlan {
        return getRawData<PointData>()->fromVariantMapWorkflow(variantMap);
    });

    plan->addSequentialStage("Load selection", [this, variantMap](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& executionContext) {
        variantMapMustContain(variantMap, "Selection");

        const auto& selectionMap = variantMap["Selection"].toMap();

        const auto count = selectionMap["Count"].value<std::uint64_t>();

        if (count > 0 && isFull()) {
            auto selectionSet = getSelection<Points>();

            if (selectionSet.isValid()) {
                selectionSet->indices.resize(count);

            	populateBytesFromBlobMap(selectionMap["Raw"].toMap(), (char*)selectionSet->indices.data(), count * sizeof(uint32_t));

                events().notifyDatasetDataSelectionChanged(this);
            }
        }
    }, WorkflowPlan::JobThreadAffinity::GuiThread);

    plan->addSequentialStage("Load indices", [this, variantMap](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& executionContext) {
        variantMapMustContain(variantMap, "Indices");

        const auto& indicesMap = variantMap["Indices"].toMap();

        indices.resize(indicesMap["Count"].value<std::uint64_t>());

        if (!indices.empty())
			populateBytesFromBlobMap(indicesMap["Raw"].toMap(), (char*)indices.data(), indices.size() * sizeof(uint32_t));
    }, WorkflowPlan::JobThreadAffinity::GuiThread);

    const auto serializeDimensionsStage = plan->addNestedWorkflowStage("Load dimensions", [this, variantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) -> UniqueWorkflowPlan {
        return DimensionNamesSerializer::fromVariantMapWorkflow(this, variantMap);
    });

    plan->addSequentialStage("Restore project resource staging policy", [restoreProjectResourceStagingPolicy](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) {
        restoreProjectResourceStagingPolicy();
    }, WorkflowPlan::JobThreadAffinity::GuiThread);

    return plan;
}

UniqueWorkflowPlan Points::toVariantMapWorkflow() const
{
    UniqueWorkflowPlan plan = std::make_unique<WorkflowPlan>(__FUNCTION__);

    const auto rawDataWeight = isFull() ? std::max(1.0, static_cast<double>(getRawData<PointData>()->getRawDataSize())) : 1.0;

    const auto saveDatasetBaseStage = plan->addNestedWorkflowStage("Save dataset base", [this](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) -> UniqueWorkflowPlan {
        return this->DatasetImpl::toVariantMapWorkflow();
    });

    WorkflowHandle encodeRawDataStage;

    if (isFull()) {
        encodeRawDataStage = plan->addNestedWorkflowStage("Encode raw data", [this](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) -> UniqueWorkflowPlan {
            return getRawData<PointData>()->toVariantMapWorkflow();
        }, WorkflowPlan::JobThreadAffinity::CurrentWorkerThread, rawDataWeight);
    }

    const auto storeRawDataStage = plan->addSequentialStage("Store raw data", [this, saveDatasetBaseStage, encodeRawDataStage](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) -> void {
        auto datasetMap = executionContext->takeOutput(saveDatasetBaseStage).toMap();

    	datasetMap["Data"]                      = isFull() ? executionContext->takeOutput(encodeRawDataStage).toMap() : QVariantMap();
        datasetMap["NumberOfPoints"]            = QVariant::fromValue<std::uint64_t>(getNumPoints());
        datasetMap["Dense"]                     = Experimental::isDense(this);
        datasetMap["NumberOfNonZeroElements"]   = QVariant::fromValue(Experimental::getNumNonZeroElements(this));

        executionContext->setOutput(datasetMap);
    });

    const auto storeIndicesStage = plan->addSequentialStage("Save indices", [this, storeRawDataStage](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& executionContext) {
        auto datasetMap = executionContext->takeOutput(storeRawDataStage).toMap();

        QVariantMap indicesMap;

        indicesMap["Count"] = QVariant::fromValue<std::uint64_t>(this->indices.size());
        indicesMap["Raw"]   = bytesToBlobVariantMap((char*)this->indices.data(), this->indices.size() * sizeof(std::uint32_t));

        datasetMap["Indices"] = indicesMap;

        executionContext->setOutput(datasetMap);
    });

    const auto saveSelectionStage = plan->addSequentialStage("Save selection", [this, storeIndicesStage](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& executionContext) {
        auto datasetMap = executionContext->takeOutput(storeIndicesStage).toMap();

        QVariantMap selection;

        if (isFull()) {
            auto selectionSet = getSelection<Points>();

            selection["Count"]  = QVariant::fromValue<std::uint64_t>(selectionSet->indices.size());
            selection["Raw"]    = bytesToBlobVariantMap((char*)selectionSet->indices.data(), selectionSet->indices.size() * sizeof(std::uint32_t));
        }

        datasetMap["Selection"] = selection;

        executionContext->setOutput(datasetMap);
    });

    const auto serializeDimensionsStage = plan->addNestedWorkflowStage("Serialize dimensions", [this](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) -> UniqueWorkflowPlan {
        return DimensionNamesSerializer::toVariantMapWorkflow(getRawData<PointData>()->getDimensionNames());
    });

    const auto saveDimensionsStage = plan->addSequentialStage("Save dimensions", [this, saveSelectionStage, serializeDimensionsStage](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& executionContext) {
        auto datasetMap = executionContext->takeOutput(saveSelectionStage).toMap();

        datasetMap["DimensionNames"]        = executionContext->takeOutput(serializeDimensionsStage).toMap();
        datasetMap["NumberOfDimensions"]    = QVariant::fromValue<std::uint64_t>(getNumDimensions());
        datasetMap["Dimensions"]            = _dimensionsPickerAction->toVariantMap();

        executionContext->setOutput(datasetMap);
    });

    return plan;
}

// =============================================================================
// Factory
// =============================================================================

PointDataFactory::PointDataFactory()
{
    setIconByName("database");
    setCategoryIconByName("database");
}

QUrl PointDataFactory::getReadmeMarkdownUrl() const
{
    return QUrl("https://raw.githubusercontent.com/ManiVaultStudio/core/master/ManiVault/src/plugins/PointData/README.md");
}

QUrl PointDataFactory::getRepositoryUrl() const
{
    return QUrl("https://github.com/ManiVaultStudio/core");
}

mv::plugin::RawData* PointDataFactory::produce()
{
    return new PointData(this);
}
