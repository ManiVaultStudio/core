// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PointData.h"
#include "PointDataLegacySerialization.h"

#include "DimensionsPickerAction.h"
#include "actions/InfoAction.h"
#include "actions/PointsAction.h"
#include "DimensionNamesSerializer.h"
#include "storage/eager/EagerPointsStorageBackend.h"
#include "storage/actions/StorageBackendConversionAction.h"
#include "storage/sparse/SparsePointsStorageBackend.h"
#include "storage/tiled/PointDataTiledSerialization.h"
#include "storage/tiled/PointDataTiledStorage.h"
#include "storage/tiled/TiledPointsStorageBackend.h"

#include <Application.h>

#include <actions/GroupAction.h>
#include <event/Event.h>
#include <graphics/Vector2f.h>

#include <util/Serialization.h>
#include <util/JSON.h>

#include <workflow/WorkflowContextVariantMap.h>

#include <QDebug>
#include <QtCore>

#include <cstring>
#include <algorithm>
#include <limits>
#include <set>
#include <stdexcept>
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
    _storageBackend(std::make_unique<mv::pointdata::storage::EagerPointsStorageBackend>(*this))
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

void PointData::init()
{
}

void PointData::refreshStorageBackend()
{
    _storageBackend = std::make_unique<mv::pointdata::storage::EagerPointsStorageBackend>(*this);
}

void PointData::clearStorageBackend()
{
    _storageBackend.reset();
}

std::shared_ptr<mv::pointdata::tiled::PointDataTiledStorage> PointData::getTiledStorage() const
{
    const auto* tiledBackend = dynamic_cast<const mv::pointdata::storage::TiledPointsStorageBackend*>(_storageBackend.get());
    return tiledBackend ? tiledBackend->getStorage() : nullptr;
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

    if (const auto tiledStorage = getTiledStorage())
        return tiledStorage->rows();
    if (_storageBackend && _storageBackend->getType() == pointdata::storage::StorageBackendType::Sparse)
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
    if (const auto tiledStorage = getTiledStorage())
        return tiledStorage->rows() * tiledStorage->columns();
    if (_storageBackend && _storageBackend->getType() == pointdata::storage::StorageBackendType::Sparse)
        return _storageBackend->getNumPoints() * _storageBackend->getNumDimensions();

    return getSizeOfVector();
}


std::uint64_t PointData::getRawDataSize() const
{
    const auto dataLock = lockData();

    if (const auto tiledStorage = getTiledStorage())
        return getNumberOfElements() * tiledStorage->elementSize();
    if (const auto sparseStorage = dynamic_cast<const pointdata::storage::SparsePointsStorageBackend*>(_storageBackend.get()))
        return sparseStorage->getStorageSizeBytes();
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

    return _storageBackend ? _storageBackend->getType() : pointdata::storage::StorageBackendType::Eager;
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

std::unique_ptr<mv::gui::StorageBackendConversionAction> PointData::createStorageBackendConversionAction(const pointdata::storage::StorageBackendType targetType, QObject* parent) const
{
    const auto lock = lockData();

    return _storageBackend ? _storageBackend->createConversionAction(targetType, parent) : nullptr;
}

void PointData::convertToStorageBackend(const pointdata::storage::StorageBackendType type, const pointdata::storage::StorageBackendConfiguration& configuration, pointdata::storage::StorageBackendProgressCallback progress)
{
    convertToStorageBackend(type, std::make_shared<const pointdata::storage::StorageBackendConfiguration>(configuration), std::move(progress));
}

void PointData::initializeTiledStorage(const std::uint64_t rows, const std::uint64_t columns, const ElementTypeSpecifier elementType, const pointdata::storage::StorageBackendConfiguration& configuration, TiledTileGenerator generator)
{
    auto creation = std::make_shared<pointdata::storage::TiledStorageBackendCreationOptions>();
    creation->rows = rows;
    creation->columns = columns;
    creation->elementSize = 4;
    switch (elementType)
    {
        case ElementTypeSpecifier::bfloat16:
        case ElementTypeSpecifier::int16:
        case ElementTypeSpecifier::uint16:
            creation->elementSize = 2;
            break;

        case ElementTypeSpecifier::int8:
        case ElementTypeSpecifier::uint8:
            creation->elementSize = 1;
            break;

        default:
            creation->elementSize = 4;
            break;
    }
    creation->elementTypeIndex = static_cast<std::uint32_t>(elementType);
    creation->rowTileSize = configuration.rowTileSize;
    creation->columnTileSize = configuration.columnTileSize;
    creation->cacheCapacity = configuration.cacheCapacity;
    creation->cacheCapacityBytes = configuration.cacheCapacityBytes;
    creation->packedTiles = configuration.packedTiles;
    creation->superTileRows = configuration.superTileRows;
    creation->superTileColumns = configuration.superTileColumns;
    creation->generator = std::move(generator);
    convertToStorageBackend(pointdata::storage::StorageBackendType::Tiled, creation);
}

void PointData::initializeSparseStorage(const std::uint64_t rows, const std::uint64_t columns, const ElementTypeSpecifier elementType, const bool columnCompressed, std::vector<std::uint64_t> pointers, std::vector<std::uint64_t> indices, std::vector<std::uint8_t> values)
{
    const auto lock = lockData();
    if (_numRows != 0 || getSizeOfVector() != 0 || getTiledStorage())
        throw std::invalid_argument("Point-data storage creation requires an empty dataset");

    const auto layout      = columnCompressed ? pointdata::storage::SparseMatrixLayout::ColumnCompressed : pointdata::storage::SparseMatrixLayout::RowCompressed;
    const auto elementSize = elementType == ElementTypeSpecifier::bfloat16 || elementType == ElementTypeSpecifier::int16 || elementType == ElementTypeSpecifier::uint16 ? 2 :
        elementType == ElementTypeSpecifier::int8 || elementType == ElementTypeSpecifier::uint8 ? 1 : 4;
    auto sparseStorage = std::make_unique<pointdata::storage::SparsePointsStorageBackend>(rows, columns, layout, static_cast<std::uint32_t>(elementType), elementSize, std::move(pointers), std::move(indices), std::move(values));
    setElementTypeSpecifier(elementType);
    _numDimensions  = columns;
    _numRows        = rows;
    _isDense        = true;
    _storageBackend = std::move(sparseStorage);
}

void PointData::attachPackedTileSource(const QStringList& filePaths, const QString& codecName, const std::vector<std::uint64_t>& offsets, const std::vector<std::uint64_t>& compressedSizes, const std::vector<std::uint64_t>& uncompressedSizes, const std::vector<std::size_t>& superTileIndices, std::shared_ptr<void> sourceLifetime)
{
    const auto tiledStorage = getTiledStorage();
    if (!tiledStorage)
        throw std::invalid_argument("Packed tile sources require tiled storage");

    if (offsets.size() != compressedSizes.size() || offsets.size() != uncompressedSizes.size() || offsets.size() != superTileIndices.size())
        throw std::invalid_argument("Packed tile source metadata has inconsistent lengths");

    std::vector<mv::util::PackedBlobBlock> blocks;
    blocks.reserve(offsets.size());
    for (std::size_t index = 0; index < offsets.size(); ++index)
        blocks.push_back({ offsets[index], compressedSizes[index], uncompressedSizes[index] });

    tiledStorage->setPackedTileSource(filePaths, codecName, blocks, superTileIndices, std::move(sourceLifetime));
}

void PointData::persistTiledStorage(pointdata::storage::StorageBackendProgressCallback progress)
{
    const auto lock = lockData();
    const auto tiledStorage = getTiledStorage();

    if (!tiledStorage)
        return;

    const auto serializedStorage = pointdata::tiled::PointDataTiledSerialization::toVariantMap(*tiledStorage, progress);
    const auto persistedStorage = pointdata::tiled::PointDataTiledSerialization::fromVariantMap(serializedStorage, tiledStorage->rows(), tiledStorage->columns(), tiledStorage->elementSize());
    dynamic_cast<pointdata::storage::TiledPointsStorageBackend*>(_storageBackend.get())->setStorage(persistedStorage);
}

UniqueWorkflowPlan PointData::persistTiledStorageWorkflow()
{
    const auto lock = lockData();
    const auto tiledStorage = getTiledStorage();

    if (!tiledStorage)
        return {};

    auto plan = std::make_unique<WorkflowPlan>("Persist tiled point data");
    const auto serializeStage = plan->addNestedWorkflowStage("Serialize tiled point data", [tiledStorage](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) {
        return pointdata::tiled::PointDataTiledSerialization::toVariantMapWorkflow(*tiledStorage);
    });

    plan->addSequentialStage("Install persisted tiled storage", [this, tiledStorage, serializeStage](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) {
        const auto serializedStorage = executionContext->takeOutput(serializeStage).toMap();
        const auto persistedStorage = pointdata::tiled::PointDataTiledSerialization::fromVariantMap(serializedStorage, tiledStorage->rows(), tiledStorage->columns(), tiledStorage->elementSize());
        const auto dataLock = lockData();
        dynamic_cast<pointdata::storage::TiledPointsStorageBackend*>(_storageBackend.get())->setStorage(persistedStorage);
    }, WorkflowPlan::JobThreadAffinity::GuiThread);

    return plan;
}

void PointData::convertToStorageBackend(const pointdata::storage::StorageBackendType type, const pointdata::storage::StorageBackendConfigurationOptionsPtr options, pointdata::storage::StorageBackendProgressCallback progress)
{
    const auto lock = lockData();

    if (type == pointdata::storage::StorageBackendType::Tiled)
    {
        const auto creation = std::dynamic_pointer_cast<const pointdata::storage::TiledStorageBackendCreationOptions>(options);

        if (creation)
        {
            if (_numRows != 0 || getSizeOfVector() != 0 || getTiledStorage())
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

        if (const auto tiledStorage = getTiledStorage())
        {
            if (tiledStorage->rowTileSize() != configuration->rowTileSize || tiledStorage->columnTileSize() != configuration->columnTileSize)
            {
                const auto retiledStorage = mv::pointdata::tiled::PointDataTiledStorage::retile(tiledStorage, configuration->rowTileSize, configuration->columnTileSize, progress);
                dynamic_cast<pointdata::storage::TiledPointsStorageBackend*>(_storageBackend.get())->setStorage(retiledStorage);
            }
        }
        else
            makeTiled(configuration->rowTileSize, configuration->columnTileSize, progress);

        if (_storageBackend)
            _storageBackend->applyConfiguration(options);

        return;
    }

    if (type == pointdata::storage::StorageBackendType::Eager)
    {
        materializeStorageBackend(std::move(progress));

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

void PointData::makeTiled(const std::size_t rowTileSize, const std::size_t columnTileSize, std::function<void(float)> progress)
{
    const auto lock = lockData();

    materializeStorageBackend();

    if (_isDense && !getTiledStorage() && _numDimensions > 0)
    {
        const auto rows = static_cast<std::uint64_t>(getSizeOfVector() / _numDimensions);

        const auto elementSize = std::visit([](const auto& vector) { return sizeof(typename std::decay_t<decltype(vector)>::value_type); }, _variantOfVectors);
        const auto* data = static_cast<const char*>(std::visit([](const auto& vector) { return static_cast<const void*>(vector.data()); }, _variantOfVectors));

        const auto tiledStorage = mv::pointdata::tiled::PointDataTiledStorage::fromDense(data, rows, _numDimensions, elementSize, rowTileSize, columnTileSize, std::move(progress));

        _numRows = rows;
        _storageBackend = std::make_unique<pointdata::storage::TiledPointsStorageBackend>(tiledStorage, static_cast<std::uint32_t>(getElementTypeSpecifier()));
    }
}

namespace
{
PointDataAccessView makeFloatAccessView(std::vector<float>&& values)
{
    auto bytes = std::make_shared<QByteArray>(reinterpret_cast<const char*>(values.data()), static_cast<qsizetype>(values.size() * sizeof(float)));
    PointDataAccessView view;
    view.data = bytes->constData();
    view.rows = 1;
    view.columns = static_cast<std::uint64_t>(values.size());
    view.rowStrideBytes = static_cast<std::uint64_t>(bytes->size());
    view.elementTypeIndex = static_cast<std::uint32_t>(PointData::ElementTypeSpecifier::float32);
    view.elementSizeBytes = sizeof(float);
    view.lifetime = std::shared_ptr<const void>(bytes, bytes->constData());
    return view;
}

}

PointDataAccessView PointData::getEagerRow(const std::uint64_t row) const
{
    if (row >= getNumPoints())
        return {};

    std::vector<float> values(static_cast<std::size_t>(_numDimensions));

    for (std::uint64_t column = 0; column < _numDimensions; ++column)
    {
        values[static_cast<std::size_t>(column)] = getEagerValueAt(static_cast<std::size_t>(row * _numDimensions + column));
    }

    return makeFloatAccessView(std::move(values));
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

void PointData::visitEagerRange(const std::uint64_t firstRow, const std::uint64_t rowCount, const std::uint64_t firstColumn, const std::uint64_t columnCount, const PointDataAccessVisitor& visitor) const
{
    if (!visitor || firstRow > getNumPoints() || rowCount > getNumPoints() - firstRow ||
        firstColumn > _numDimensions || columnCount > _numDimensions - firstColumn)
        throw std::out_of_range("Point-data range is invalid");
    if (rowCount == 0 || columnCount == 0)
        return;

    for (std::uint64_t row = firstRow; row < firstRow + rowCount; ++row)
    {
        std::vector<float> values(static_cast<std::size_t>(columnCount));

        for (std::uint64_t column = 0; column < columnCount; ++column)
            values[static_cast<std::size_t>(column)] = getEagerValueAt(static_cast<std::size_t>(row * _numDimensions + firstColumn + column));

        visitor(makeFloatAccessView(std::move(values)));
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

void PointData::materializeTiled(pointdata::storage::StorageBackendProgressCallback progress)
{
    const auto tiledStorage = getTiledStorage();

    if (!tiledStorage)
        return;

    const auto elementCount = static_cast<std::size_t>(tiledStorage->rows() * tiledStorage->columns());
    const auto elementSize  = tiledStorage->elementSize();
    std::vector<char> materializedBytes(elementCount * elementSize);

    tiledStorage->materialize(materializedBytes.data(), [&progress](const float tileProgress) {
        if (progress)
            progress(tileProgress * 0.95f);
    });

    resizeVector(elementCount);
    std::visit([&materializedBytes](auto& vector) {
        if (!vector.empty())
            std::memcpy(vector.data(), materializedBytes.data(), vector.size() * sizeof(typename std::decay_t<decltype(vector)>::value_type));
    }, _variantOfVectors);

    clearStorageBackend();
    refreshStorageBackend();

    if (progress)
        progress(1.0f);
}

void PointData::materializeStorageBackend(pointdata::storage::StorageBackendProgressCallback progress)
{
    if (getTiledStorage())
    {
        materializeTiled(std::move(progress));
        return;
    }

    if (!_storageBackend || _storageBackend->getType() == pointdata::storage::StorageBackendType::Eager)
        return;

    const auto rows        = _storageBackend->getNumPoints();
    const auto columns     = _storageBackend->getNumDimensions();
    const auto elementSize = _storageBackend->getElementSize();
    if (rows != 0 && columns > std::numeric_limits<std::uint64_t>::max() / rows)
        throw std::overflow_error("Sparse matrix dimensions overflow the dense element count");

    const auto elementCount = rows * columns;
    if (elementSize == 0 || elementCount > std::numeric_limits<std::size_t>::max() / elementSize)
        throw std::overflow_error("Sparse matrix is too large to materialize");

    std::vector<std::uint8_t> materializedBytes(static_cast<std::size_t>(elementCount) * elementSize);
    _storageBackend->materialize(reinterpret_cast<char*>(materializedBytes.data()));
    setElementTypeSpecifier(static_cast<ElementTypeSpecifier>(_storageBackend->getElementTypeIndex()));
    resizeVector(static_cast<std::size_t>(elementCount));
    std::visit([&materializedBytes](auto& vector) {
        if (!vector.empty())
            std::memcpy(vector.data(), materializedBytes.data(), materializedBytes.size());
    }, _variantOfVectors);
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

float PointData::getEagerValueAt(const std::size_t index) const
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
        _isDense            = variantMap.value("Dense", true).toBool();
        _numDimensions      = static_cast<std::size_t>(variantMap["NumberOfDimensions"].toULongLong());
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
            initializeSparseStorage(rows, _numDimensions, elementType, layout == static_cast<int>(pointdata::storage::SparseMatrixLayout::ColumnCompressed), std::move(pointers), std::move(indices), std::move(values));
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

    {
        const auto dataLock = lockData();
        isDense = _isDense;
        tiledStorage = getTiledStorage();
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

        plan->addSequentialStage("Build tiled point data map", [this, serializeTiledStage](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) {
            const auto dataLock = lockData();
            QVariantMap outputMap;

            outputMap["TypeIndex"]        = static_cast<std::int32_t>(getElementTypeSpecifier());
            outputMap["TypeName"]         = elementTypeName(getElementTypeSpecifier());
            outputMap["Tiled"]            = executionContext->takeOutput(serializeTiledStage).toMap();
            outputMap["NumberOfElements"] = QVariant::fromValue(static_cast<qulonglong>(getNumberOfElements()));

            executionContext->setOutput(outputMap);
        });
    }
    else if (isSparse)
    {
        plan->addSequentialStage("Serialize sparse point data", [this](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) {
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

        plan->addSequentialStage("Build map", [this, storeRawStage](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) {
            const auto dataLock = lockData();
            QVariantMap outputMap;

            const auto typeSpecifier        = getElementTypeSpecifier();
            const auto typeSpecifierName    = getElementTypeNames()[static_cast<std::int32_t>(typeSpecifier)];
            const auto typeIndex            = static_cast<std::int32_t>(typeSpecifier);
            const auto rawMap               = executionContext->takeOutput(storeRawStage).toMap();

            outputMap.insert("TypeIndex", QVariant::fromValue(typeIndex));
            outputMap.insert("TypeName", QVariant(typeSpecifierName));
            outputMap.insert("Raw", rawMap);
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
        plan->addSequentialStage("Build map", [this](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) {
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

    if (_storageBackend && _storageBackend->getType() != pointdata::storage::StorageBackendType::Eager)
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

        if (_storageBackend && _storageBackend->getType() != pointdata::storage::StorageBackendType::Eager)
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

    if (_storageBackend && _storageBackend->getType() != pointdata::storage::StorageBackendType::Eager)
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

    if (_storageBackend && _storageBackend->getType() != pointdata::storage::StorageBackendType::Eager)
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

        for (const auto& selectionIndex : selectionIndices) {
            selected[localGlobalIndices[selectionIndex]] = true;
        }
    }
    else {
        // In an array the size of the full raw data, mark selected points as true
        std::vector<bool> globalSelection(getSourceDataset<Points>()->getNumRawPoints(), false);

        for (const std::uint32_t& selectionIndex : selectionIndices)
            globalSelection[selectionIndex] = true;

        // For all local points find out which are selected
        selected.resize(localGlobalIndices.size(), false);
        for (std::uint64_t i = 0; i < localGlobalIndices.size(); i++)
        {
            if (globalSelection[localGlobalIndices[i]])
                selected[i] = true;
        }
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

std::shared_ptr<pointdata::tiled::PointDataTiledStorage> Points::getTiledStorage() const
{
    return isProxy() ? nullptr : getRawData<PointData>()->getTiledStorage();
}

void Points::setTiledLoaderThreadSafe(const bool threadSafe)
{
    if (!isProxy())
    {
        if (const auto tiledStorage = getRawData<PointData>()->getTiledStorage())
            tiledStorage->setLoaderThreadSafe(threadSafe);
    }
}

void Points::convertToStorageBackend(const StorageBackendType type, const StorageBackendConfiguration& configuration, StorageBackendProgressCallback progress)
{
    if (isProxy())
        throw std::invalid_argument("Storage backend conversion is only supported for concrete point datasets");

    getRawData<PointData>()->convertToStorageBackend(type, configuration, std::move(progress));
    mv::events().notifyDatasetDataChanged(this);
}

void Points::initializeTiledStorage(const std::uint64_t rows, const std::uint64_t columns, const ElementTypeSpecifier elementType, const StorageBackendConfiguration& configuration, TiledTileGenerator generator)
{
    if (isProxy())
        throw std::invalid_argument("Storage backend initialization is only supported for concrete point datasets");

    getRawData<PointData>()->initializeTiledStorage(rows, columns, elementType, configuration, std::move(generator));
    mv::events().notifyDatasetDataChanged(this);
}

void Points::initializeSparseStorage(const std::uint64_t rows, const std::uint64_t columns, const ElementTypeSpecifier elementType, const bool columnCompressed, std::vector<std::uint64_t> pointers, std::vector<std::uint64_t> indices, std::vector<std::uint8_t> values)
{
    if (isProxy())
        throw std::invalid_argument("Storage backend initialization is only supported for concrete point datasets");

    getRawData<PointData>()->initializeSparseStorage(rows, columns, elementType, columnCompressed, std::move(pointers), std::move(indices), std::move(values));
    mv::events().notifyDatasetDataChanged(this);
}

void Points::attachPackedTileSource(const QStringList& filePaths, const QString& codecName, const std::vector<std::uint64_t>& offsets, const std::vector<std::uint64_t>& compressedSizes, const std::vector<std::uint64_t>& uncompressedSizes, const std::vector<std::size_t>& superTileIndices, std::shared_ptr<void> sourceLifetime)
{
    if (isProxy())
        throw std::invalid_argument("Packed tile sources can only be attached to concrete point datasets");

    getRawData<PointData>()->attachPackedTileSource(filePaths, codecName, offsets, compressedSizes, uncompressedSizes, superTileIndices, std::move(sourceLifetime));
}

void Points::persistTiledStorage(StorageBackendProgressCallback progress)
{
    if (isProxy())
        throw std::invalid_argument("Tiled storage persistence is only supported for concrete point datasets");

    getRawData<PointData>()->persistTiledStorage(std::move(progress));
    mv::events().notifyDatasetDataChanged(this);
}

UniqueWorkflowPlan Points::persistTiledStorageWorkflow()
{
    if (isProxy())
        throw std::invalid_argument("Tiled storage persistence is only supported for concrete point datasets");

    return getRawData<PointData>()->persistTiledStorageWorkflow();
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

    if (appVersion < Version(1, 5, 0)) {
        plan->addSequentialStage("Load legacy points raw data (< 1.5.0)", [this, variantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) {
            legacy::PointsLegacySerializer::fromVariantMapPre150(*this, variantMap, executionContext);
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
