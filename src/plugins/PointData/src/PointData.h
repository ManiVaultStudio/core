// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "pointdata_export.h"

#include "RawData.h"

#include "LinkedData.h"
#include "PointDataRange.h"
#include "Set.h"
#include "PointDataAccess.h"
#include "SparseMatrix.h"
#include "storage/PointsStorageBackendTypes.h"

#include "event/EventListener.h"

#include <biovault_bfloat16/biovault_bfloat16.h>

#include <QByteArray>
#include <QDebug>
#include <QMap>
#include <QString>
#include <QVariantMap>

#include <array>
#include <cassert>
#include <memory>
#include <mutex>
#include <utility>
#include <variant>
#include <vector>

class QObject;

namespace mv
{
    namespace pointdata::tiled
    {
        class PointDataTiledStorage;
        class TileView;
    }

    namespace pointdata::storage
    {
        class EagerPointsStorageBackend;
        class PointsStorageBackend;
        class TiledPointsStorageBackend;
    }

    // From "graphics/Vector2f.h"
    class Vector2f;

    namespace gui {
        class GroupAction;
        class StorageBackendConversionAction;
    }

    namespace legacy
    {
        class PointDataLegacySerializer;
        class PointsLegacySerializer;
    }
}

// =============================================================================
// Data Type
// =============================================================================

const mv::DataType PointType = mv::DataType(QString("Points"));

class InfoAction;
class PointsAction;
class DimensionsPickerAction;
class ClusterAction;

// =============================================================================
// Raw Data
// =============================================================================

/**
 * @brief Dense point-data storage and access interface.
 *
 * PointData is the storage-neutral entry point for datasets whose values are
 * arranged as rows of points and columns of dimensions. It retains the
 * established eager vector representation for compatibility with existing
 * consumers, while also providing an experimental tiled representation for
 * datasets that are too large to load and decode as one contiguous buffer.
 *
 * The tiled functionality is deliberately exposed through this class without
 * making the tiled implementation part of the public header. This keeps the
 * common PointData include lightweight and allows other storage backends to
 * implement the same row, column, range, and visitor APIs in the future.
 * Tiled storage is currently read-only; legacy raw-pointer and mutation APIs
 * materialize tiled data when a contiguous eager buffer is required.
 *
 * @author Thomas Kroes
 */
class POINTDATA_EXPORT PointData : public mv::plugin::RawData
{
public:
    using TileView = mv::pointdata::tiled::TileView;
    using DataLock = std::unique_lock<std::recursive_mutex>;
    /** Generates the row-major bytes for one requested tile. */
    using TiledTileGenerator = std::function<QByteArray(std::uint64_t rowOffset, std::uint64_t columnOffset, std::uint64_t rows, std::uint64_t columns)>;

    enum class ElementTypeSpecifier
    {
        float32,
        bfloat16,
        int32,
        uint32,
        int16,
        uint16,
        int8,
        uint8
    };

    static ElementTypeSpecifier elementTypeSpecifier(const QString& typeName);
    static QString elementTypeName(const ElementTypeSpecifier elementTypeSpecifier);

private:
    using VariantOfVectors = std::variant <
        std::vector<float>,
        std::vector<biovault::bfloat16_t>,
        std::vector<std::int32_t>,
        std::vector<std::uint32_t>,
        std::vector<std::int16_t>,
        std::vector<std::uint16_t>,
        std::vector<std::int8_t>,
        std::vector<std::uint8_t> >;

    // Sets the index of the specified variant. If the new index is different from the previous one, the value will be reset. 
    // Inspired by `expand_type` from kmbeutel at
    // https://www.reddit.com/r/cpp/comments/f8cbzs/creating_stdvariant_based_on_index_at_runtime/?rdt=52905
    template <typename... Alternatives>
    static void setIndexOfVariant(std::variant<Alternatives...>& var, std::size_t index)
    {
        if (index != var.index())
        {
            assert(index < sizeof...(Alternatives));
            const std::variant<Alternatives...> variants[] = { Alternatives{ }... };
            var = variants[index];
        }
    }


    // Returns the index of the specified alternative: the position of the alternative type withing the variant.
    // Inspired by `variant_index` from Bargor at
    // https://stackoverflow.com/questions/52303316/get-index-by-type-in-stdvariant
    template<typename Variant, typename Alternative, std::size_t index = 0>
    static constexpr std::size_t getIndexOfVariantAlternative()
    {
        static_assert(index < std::variant_size_v<Variant>);

        if constexpr (std::is_same_v<std::variant_alternative_t<index, Variant>, Alternative>)
        {
            return index;
        }
        else
        {
            return getIndexOfVariantAlternative<Variant, Alternative, index + 1>();
        }
    }

    template <typename T>
    static constexpr ElementTypeSpecifier getElementTypeSpecifier()
    {
        constexpr auto index = getIndexOfVariantAlternative<VariantOfVectors, std::vector<T>>();
        return static_cast<ElementTypeSpecifier>(index);
    }

    template <typename T>
    const std::vector<T>& getConstVector() const
    {
        // This function should only be used to access the currently selected vector.
        assert(std::holds_alternative<std::vector<T>>(_variantOfVectors));
        return std::get<std::vector<T>>(_variantOfVectors);
    }

    template <typename T>
    const std::vector<T>& getVector() const
    {
        return getConstVector<T>();
    }

    template <typename T>
    std::vector<T>& getVector()
    {
        return const_cast<std::vector<T>&>(getConstVector<T>());
    }

    /// Returns the size of the std::vector currently held by _variantOfVectors.
    std::size_t getSizeOfVector() const
    {
        return std::visit([](const auto& vec) { return vec.size(); }, _variantOfVectors);
    }

    /// Resizes the std::vector currently held by _variantOfVectors.
    void resizeVector(const std::size_t newSize)
    {
        std::visit([newSize](auto& vec) { vec.resize(newSize); }, _variantOfVectors);
    }

    void setElementTypeSpecifier(const ElementTypeSpecifier elementTypeSpecifier)
    {
        setIndexOfVariant(_variantOfVectors, static_cast<std::size_t>(elementTypeSpecifier));
    }

    ElementTypeSpecifier getElementTypeSpecifier() const
    {
        return static_cast<ElementTypeSpecifier>(_variantOfVectors.index());
    }

    /// Resizes the currently held data vector to the specified
    /// number of elements, and converts the elements of the specified data
    /// to the internal data element type, by static_cast. 
    template <typename T>
    void convertData(const T* const data, const std::size_t numberOfElements)
    {
        std::visit([data, numberOfElements](auto& vec)
        {
            vec.resize(numberOfElements);

            std::size_t i{};
            for (auto& elem: vec)
            {
                elem = static_cast<std::remove_reference_t<decltype(elem)>>(data[i]);
                ++i;
            }
        },
        _variantOfVectors);
    }


    template <typename DimensionIndex>
    void CheckDimensionIndex(const DimensionIndex& dimensionIndex) const
    {
        assert(dimensionIndex >= 0);
        assert(static_cast<std::uintmax_t>(dimensionIndex) < _numDimensions);
    }

    template <typename DimensionIndices>
    void CheckDimensionIndices(const DimensionIndices& dimensionIndices) const
    {
        for (const auto dimensionIndex : dimensionIndices)
        {
            CheckDimensionIndex(dimensionIndex);
        }
    }

public:
    /// Yields the n-th supported element type. Corresponds to the n-th entry
    /// in the array of type names, returned by getElementTypeNames().
    template <std::size_t N>
    using ElementTypeAt = typename std::variant_alternative_t<N, VariantOfVectors>::value_type;

    PointData(mv::plugin::PluginFactory* factory);
    ~PointData() override;

    /**
     * Acquires exclusive access to the point-data storage and its metadata.
     *
     * Keep the returned lock alive for the complete lifetime of any pointer,
     * iterator or reference obtained from this PointData instance.
     */
    [[nodiscard]] DataLock lockData() const
    {
        return DataLock(_dataMutex);
    }

    void init() override;

    mv::Dataset<mv::DatasetImpl> createDataSet(const QString& guid = "") const override;

    std::uint64_t getNumPoints() const;

    std::uint64_t getNumDimensions() const;

    std::uint64_t getNumberOfElements() const;

    /**
     * Get amount of data occupied by the raw data
     * @return Size of the raw data in bytes
     */
    std::uint64_t getRawDataSize() const override;

    /** Get the name of the currently stored element type. */
    QString getElementTypeName() const { return elementTypeName(getElementTypeSpecifier()); }

    /**
     *Returns void pointer to the underlying array serving as element storage.
     */
    void* getDataVoidPtr();
    const void* getDataConstVoidPtr() const;

    /** Returns whether the point data currently uses tiled storage. */
    bool isTiled() const;

    /** Returns the physical storage backend used by this concrete dataset. */
    mv::pointdata::storage::StorageBackendType getStorageBackendType() const;

    /** Returns the backend capability flags for this concrete dataset. */
    mv::pointdata::storage::StorageBackendCapabilities getStorageBackendCapabilities() const;

    /** Creates controls for converting to the requested storage backend. */
    std::unique_ptr<mv::gui::StorageBackendConversionAction> createStorageBackendConversionAction(mv::pointdata::storage::StorageBackendType targetType, QObject* parent = nullptr) const;

    /**
     * Converts this concrete dataset to the requested storage backend.
     *
     * Conversion is transactional: the current backend remains active until
     * the requested backend has been created successfully. The experimental
     * implementation currently supports Eager-to-Tiled conversion and
     * materialization back to Eager storage.
     */
    void convertToStorageBackend(mv::pointdata::storage::StorageBackendType type, const mv::pointdata::storage::StorageBackendConfiguration& configuration = {}, mv::pointdata::storage::StorageBackendProgressCallback progress = {});

    /** Converts using backend-specific options supplied by a conversion action. */
    void convertToStorageBackend(mv::pointdata::storage::StorageBackendType type, mv::pointdata::storage::StorageBackendConversionOptionsPtr options, mv::pointdata::storage::StorageBackendProgressCallback progress = {});

    /**
     * Applies options owned by the current storage backend without converting
     * the point data to another backend.
     *
     * The options must be compatible with the active backend. The operation
     * throws std::invalid_argument when no backend is available or the backend
     * rejects the supplied options.
     */
    void configureStorageBackend(mv::pointdata::storage::StorageBackendConversionOptionsPtr options);

    /**
     * Converts dense point data to read-only rectangular tiled storage.
     *
     * The eager vector remains the source during conversion and is replaced
     * by a resident tile index after the operation. Existing raw-pointer APIs
     * can still materialize that representation when needed. The progress
     * callback receives values in the range [0, 1].
     */
    void makeTiled(std::size_t rowTileSize, std::size_t columnTileSize, std::function<void(float)> progress = {});

    /**
     * Creates read-only tiled storage whose tile bytes are generated on demand.
     *
     * The generator is called only when a tile is first requested and must
     * return row-major bytes for the descriptor supplied to it. This is
     * intended for procedural or externally indexed data and does not create
     * an eager backing vector.
     */
    void makeTiledFromGenerator(std::uint64_t rows, std::uint64_t columns, ElementTypeSpecifier elementType, std::size_t rowTileSize, std::size_t columnTileSize, TiledTileGenerator generator);

    /**
     * Loads or retrieves a tile and returns a lifetime-safe read-only view.
     *
     * The returned view owns a lease on the tile payload, so it remains valid
     * even if the bounded cache evicts the tile before the view is released.
     * Invalid tile coordinates return an empty view.
     */
    TileView acquireTile(std::uint64_t tileRow, std::uint64_t tileColumn) const;

    /** Returns the number of currently cached tile payloads. */
    std::size_t getTiledCacheSize() const;

    /** Returns the maximum number of cached tile payloads. */
    std::size_t getTiledCacheCapacity() const;

    /** Returns the configured number of rows per tile. */
    std::size_t getTiledRowTileSize() const;

    /** Returns the configured number of columns per tile. */
    std::size_t getTiledColumnTileSize() const;

    /** Returns the number of tile-cache hits. */
    std::uint64_t getTiledCacheHits() const;

    /** Returns the number of tile-cache misses. */
    std::uint64_t getTiledCacheMisses() const;

    /** Returns the number of tile-cache evictions. */
    std::uint64_t getTiledCacheEvictions() const;

    /** Estimate raw, largest-tile, cache, descriptor, and tile-count requirements without allocation. */
    struct TiledMemoryEstimate
    {
        std::uint64_t rawDataBytes      = 0; /**< Logical dense matrix size. */
        std::uint64_t largestTileBytes = 0; /**< Payload size of the largest possible tile. */
        std::uint64_t cachedTileBytes  = 0; /**< Conservative payload size for the configured cache. */
        std::uint64_t tileCount         = 0; /**< Number of row/column tiles. */
        std::uint64_t indexBytes        = 0; /**< Conservative resident descriptor/index size. */
    };
    /** Returns a memory estimate for the current tiled storage. */
    TiledMemoryEstimate getTiledMemoryEstimate() const;

    /** Estimates tiled memory usage without allocating point data. */
    static TiledMemoryEstimate estimateTiledMemory(std::uint64_t rows, std::uint64_t columns, std::size_t elementSize, std::size_t rowTileSize, std::size_t columnTileSize, std::size_t cacheCapacity);

    /** Returns one row as a storage-neutral read-only view, or an empty view when row is out of bounds. */
    PointDataAccessView getRow(std::uint64_t row) const;

    /** Visits a contiguous range of rows in row-major order and throws for invalid bounds. */
    void visitRows(std::uint64_t firstRow, std::uint64_t rowCount, const PointDataAccessVisitor& visitor) const;

    /** Visits selected rows in the order given by rowIds; invalid IDs throw. */
    void visitRows(const std::vector<std::uint64_t>& rowIds, const PointDataAccessVisitor& visitor) const;

    /** Visits a contiguous column range for every row and throws for invalid bounds. */
    void visitColumns(std::uint64_t firstColumn, std::uint64_t columnCount, const PointDataAccessVisitor& visitor) const;

    /** Visits selected columns for every row in columnIds order; invalid IDs throw. */
    void visitColumns(const std::vector<std::uint64_t>& columnIds, const PointDataAccessVisitor& visitor) const;

    /** Visits a contiguous column range for selected rows; invalid row IDs throw. */
    void visitColumns(std::uint64_t firstColumn, std::uint64_t columnCount, const std::vector<std::uint64_t>& rowIds, const PointDataAccessVisitor& visitor) const;

    /** Visits selected columns for selected rows, grouping contiguous columns where possible. */
    void visitColumns(const std::vector<std::uint64_t>& columnIds, const std::vector<std::uint64_t>& rowIds, const PointDataAccessVisitor& visitor) const;

    /** Visits a rectangular row and column range without materializing the complete dataset. */
    void visitRange(std::uint64_t firstRow, std::uint64_t rowCount, std::uint64_t firstColumn, std::uint64_t columnCount, const PointDataAccessVisitor& visitor) const;

    static constexpr std::array<const char*, std::variant_size_v<VariantOfVectors>> getElementTypeNames()
    {
        return
        { {
            "float32",
            "bfloat16",
            "int32",
            "uint32",
            "int16",
            "uint16",
            "int8",
            "uint8"
        } };
    }

    // Similar to C++17 std::visit.
    template <typename ReturnType = void, typename FunctionObject>
    ReturnType constVisitFromBeginToEnd(FunctionObject functionObject) const
    {
        const auto dataLock = lockData();

        return std::visit([functionObject](const auto& vec) -> ReturnType
            {
                return functionObject(std::cbegin(vec), std::cend(vec));
            },
            _variantOfVectors);
    }

    // Similar to C++17 std::visit.
    template <typename ReturnType = void, typename FunctionObject>
    ReturnType visitFromBeginToEnd(FunctionObject functionObject)
    {
        const auto dataLock = lockData();

        return std::visit([functionObject](auto& vec) -> ReturnType
            {
                return functionObject(std::begin(vec), std::end(vec));
            },
            _variantOfVectors);
    }

    void extractFullDataForDimension(std::vector<float>& result, const int dimensionIndex) const;
    void extractFullDataForDimensions(std::vector<mv::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2) const;
    void extractDataForDimension(std::vector<float> &result, const int dimensionIndex, const std::vector<std::uint32_t> &indices) const;
    void extractDataForDimensions(std::vector<mv::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2, const std::vector<std::uint32_t>& indices) const;
    
    template <typename ResultContainer, typename DimensionIndices>
    void populateFullDataForDimensions(ResultContainer& resultContainer, const DimensionIndices& dimensionIndices) const
    {
        const auto dataLock = lockData();

        CheckDimensionIndices(dimensionIndices);
        std::visit([&resultContainer, this, &dimensionIndices](const auto& vec)
            {
                const std::uint64_t numPoints{ getNumPoints() };
                std::uint64_t resultIndex{};

                for (std::uint64_t pointIndex{}; pointIndex < numPoints; ++pointIndex)
                {
                    const std::uint64_t n{ pointIndex * _numDimensions };

                    for (const std::uint64_t dimensionIndex : dimensionIndices)
                    {
                        resultContainer[resultIndex] = vec[n + dimensionIndex];
                        ++resultIndex;
                    }
                }
            },
            _variantOfVectors);
    }

    template <typename ResultContainer, typename DimensionIndices, typename Indices>
    void populateDataForDimensions(ResultContainer& resultContainer, const DimensionIndices& dimensionIndices, const Indices& indices) const
    {
        const auto dataLock = lockData();

        CheckDimensionIndices(dimensionIndices);

        std::visit([&resultContainer, this, &dimensionIndices, &indices](const auto& vec)
            {
                const std::uint64_t numPoints{ static_cast<std::uint64_t>(indices.size()) };
                std::uint64_t resultIndex{};

                for (std::uint64_t pointIndex{}; pointIndex < numPoints; ++pointIndex)
                {
                    const std::uint64_t n{ indices[pointIndex] * _numDimensions };

                    for (const std::uint64_t dimensionIndex : dimensionIndices)
                    {
                        resultContainer[resultIndex] = vec[n + dimensionIndex];
                        ++resultIndex;
                    }
                }
            },
            _variantOfVectors);
    }

    const std::vector<QString>& getDimensionNames() const;

    /// Returns the number of types, supported as element type of the internal data storage. 
    static constexpr auto getNumberOfSupportedElementTypes()
    {
        return getElementTypeNames().size();
    }

    ElementTypeSpecifier getElementType() const
    {
        const auto dataLock = lockData();
        return getElementTypeSpecifier();
    }

    void setElementType(const ElementTypeSpecifier elementTypSpecifier)
    {
        const auto dataLock = lockData();
        setElementTypeSpecifier(elementTypSpecifier);
    }

    template <typename T>
    void setElementType()
    {
        const auto dataLock = lockData();
        constexpr auto elementTypSpecifier = getElementTypeSpecifier<T>();
        setElementType(elementTypSpecifier);
    }

    /// Converts the specified data to the internal data, using static_cast for
    /// each data element. Sets the number of dimensions as specified. Ensures
    /// that the size of the internal data buffer corresponds to the number of
    /// points.
    /// \note This function does not affect the selected internal data type.
    template <typename T>
    void convertData(const T* const data, const std::size_t numPoints, const std::size_t numDimensions)
    {
        const auto dataLock = lockData();

        _tiledStorage.reset();
        convertData(data, numPoints * numDimensions);
        _numDimensions = static_cast<std::uint64_t>(numDimensions);
        refreshStorageBackend();
    }

    /// Converts the specified data to the internal data, using static_cast for each data element.
    /// Convenience overload, allowing an std::vector or an std::array as
    /// input data container.
    template <typename T>
    void convertData(const T& inputDataContainer, const std::size_t numDimensions)
    {
        const auto dataLock = lockData();

        _tiledStorage.reset();
        convertData(inputDataContainer.data(), inputDataContainer.size());
        _numDimensions = static_cast<std::uint64_t>(numDimensions);
        refreshStorageBackend();
    }

    /// Copies the specified data into the internal data, sets the number of
    /// dimensions as specified, and sets the selected internal data type
    /// according to the specified data type T.
    template <typename T>
    void setData(const T* const data, const std::size_t numPoints, const std::size_t numDimensions)
    {
         const auto dataLock = lockData();

         _tiledStorage.reset();
         _variantOfVectors = VariantOfVectors( std::vector<T>(data, data + numPoints * numDimensions) );
         _numDimensions = static_cast<std::uint64_t>(numDimensions);
         refreshStorageBackend();
    }


    /// Convenience overload to allow clearing the data by setData(nullptr, 0, numDimensions). 
    void setData(std::nullptr_t data, std::size_t numPoints, std::size_t numDimensions);


    /// Copies the data from the specified vector into the internal data, sets
    /// the number of dimensions as specified, and sets the selected internal
    /// data type according to the specified data type T.
    template <typename T>
    void setData(const std::vector<T>& data, const std::size_t numDimensions)
    {
        const auto dataLock = lockData();

        _tiledStorage.reset();
        _variantOfVectors = VariantOfVectors(data);
        _numDimensions = static_cast<std::uint64_t>(numDimensions);
        refreshStorageBackend();
    }

    /// Efficiently "moves" the data from the specified vector into the internal
    /// data, sets the number of dimensions as specified, and sets the selected
    /// internal data type according to the specified data type T.
    template <typename T>
    void setData(std::vector<T>&& data, const std::size_t numDimensions)
    {
        const auto dataLock = lockData();

        _tiledStorage.reset();
        _variantOfVectors = VariantOfVectors(std::move(data));
        _numDimensions = static_cast<std::uint64_t>(numDimensions);
        refreshStorageBackend();
    }

    void setDimensionNames(const std::vector<QString>& dimNames);

    // Returns the value of the element at the specified position in the current
    // data vector, converted to float.
    // Will work fine, even when the internal data element type is not float.
    // However, may not perform well when retrieving a large number of values.
    float getValueAt(std::size_t index) const;

    // Sets the value of the element at the specified position in the current
    // data vector, converted to the internal data element type. 
    // Will work fine, even when the internal data element type is not float.
    // However, may not perform well when setting a large number of values.
    void setValueAt(std::size_t index, float newValue);

public: // Sparse data, test implementation
    class Experimental {
        friend class PointData;
    public:

        static void setSparseData(PointData* points, size_t numRows, size_t numCols, const std::vector<size_t>& rowPointers, const std::vector<size_t>& colIndices, const std::vector<float>& values)
        {
            const auto dataLock = points->lockData();
            points->_sparseData.setData(numRows, numCols, rowPointers, colIndices, values);
            points->_numRows = static_cast<uint64_t>(numRows);
            points->setData(std::vector<float> {}, numCols);
            points->_isDense = false;
        }

        static void setSparseData(PointData* points, size_t numRows, size_t numCols, std::vector<size_t>&& rowPointers, std::vector<size_t>&& colIndices, std::vector<float>&& values)
        {
            const auto dataLock = points->lockData();
            points->_sparseData.setData(numRows, numCols, std::move(rowPointers), std::move(colIndices), std::move(values));
            points->_numRows = static_cast<uint64_t>(numRows);
            points->setData(std::vector<float> {}, numCols);
            points->_isDense = false;
        }

        static SparseMatrix<size_t, size_t, float>& getSparseData(PointData* points)
        {
            // The returned reference is only safe while the caller retains a
            // lock acquired with PointData::lockData().
            return points->_sparseData; 
        }

        static bool isDense(const PointData* points)
        {
            const auto dataLock = points->lockData();
            return points->_isDense;
        }

        static void setIsDense(PointData* points, bool isDense)
        {
            const auto dataLock = points->lockData();
            points->_isDense = isDense;
        }

        static size_t getNumNonZeroElements(const PointData* points)
        {
            const auto dataLock = points->lockData();
            return points->_sparseData.getNumNonZeros();
        }

        static std::vector<float> row(const PointData* points, size_t rowIndex)
        {
            const auto dataLock = points->lockData();

            if (points->_isDense)
            {
                qWarning() << ".row() not implemented for dense data";
                return {};
            }

            return points->_sparseData.getDenseRow(rowIndex);
        }
    };

public: // Serialization

    /**
     * Create a workflow that restores this object's state from a variant map.
     *
     * See Serializable::fromVariantMapWorkflow() for the full contract,
     * execution semantics, and implementation requirements.
     *
     * @param variantMap Serialized object state.
     * @return Workflow plan that restores the object state when executed.
     */
    mv::workflow::UniqueWorkflowPlan fromVariantMapWorkflow(QVariantMap variantMap) override;

    /**
     * Create a workflow that serializes this object's state to a variant map.
     *
     * See Serializable::toVariantMapWorkflow() for the full contract,
     * execution semantics, and implementation requirements.
     *
     * @return Workflow plan that serializes the object state when executed.
     */
    mv::workflow::UniqueWorkflowPlan toVariantMapWorkflow() const final;

private:
    void refreshStorageBackend();
    PointDataAccessView getEagerRow(std::uint64_t row) const;
    void visitEagerRange(std::uint64_t firstRow, std::uint64_t rowCount, std::uint64_t firstColumn, std::uint64_t columnCount, const PointDataAccessVisitor& visitor) const;
    float getEagerValueAt(std::size_t index) const;

    void materializeTiled();
    mutable std::recursive_mutex _dataMutex;

    VariantOfVectors _variantOfVectors;

    /** Number of features of each data point */
    std::uint64_t _numDimensions = 1;
    std::shared_ptr<mv::pointdata::tiled::PointDataTiledStorage> _tiledStorage;
    std::unique_ptr<mv::pointdata::storage::PointsStorageBackend> _storageBackend;

    std::vector<QString> _dimNames;

private: // Sparse data, experimental
    std::uint64_t                    _numRows    = 0;
    SparseMatrix<size_t, size_t, float> _sparseData = {};

    bool _isDense = true;

    friend class mv::legacy::PointDataLegacySerializer;
    friend class mv::pointdata::storage::EagerPointsStorageBackend;
};

// =============================================================================
// Point Set
// =============================================================================

/**
 * @brief Logical point-data dataset and storage facade.
 *
 * Points is the public dataset-level facade for PointData. It represents a
 * complete point set, a subset selected by source indices, or a proxy that
 * combines several point datasets. The facade keeps consumers independent of
 * whether a concrete member uses eager vectors or read-only tiled storage and
 * translates logical row IDs when subsets or proxies are involved.
 *
 * The tiled and visitor APIs are intentionally exposed here first as an
 * experimental storage-neutral access layer. Consumers should use these
 * functions instead of reaching through the dataset to its PointData object.
 * Returned views are read-only and retain their payload through their lifetime
 * token; legacy raw-pointer and mutation paths preserve their existing eager
 * materialization behavior.
 *
 * @author Thomas Kroes
 */
class POINTDATA_EXPORT Points : public mv::DatasetImpl
{
private:
    /* Private helper function for visitData. Helps to reduces duplicate
    * code between const and non-const overloads of visitData.
    */
    template <typename ReturnType = void, typename FunctionObject>
    static ReturnType privateVisitData(Points& points, const FunctionObject functionObject)
    {
        return points.template visitFromBeginToEnd<ReturnType>(
                [&points, functionObject](const auto begin, const auto end) -> ReturnType
                {
                    const auto numberOfDimensions = points.getNumDimensions();

                    if (points.isFull())
                    {
                        const auto indexFunction = [](const auto index)
                        {
                            // Simply return the index value that is passed as argument.
                            return index;
                        };

                        return functionObject(mv::makePointDataRangeOfFullSet(
                            begin, end, numberOfDimensions, indexFunction));
                    }
                    else
                    {
                        // In this case, this Points object represents a subset.
                        const auto indexFunction = [](const auto indexIterator)
                        {
                            // Get the index by dereferencing the iterator.
                            return *indexIterator;
                        };

                        return functionObject(mv::makePointDataRangeOfSubset(
                            begin, points.indices, numberOfDimensions, indexFunction));
                    }
                });
    }


    /* Private helper function for visitSourceData. Helps to reduces duplicate
    * code between const and non-const overloads of visitSourceData.
    */
    template <typename ReturnType = void, typename FunctionObject>
    static ReturnType privateVisitSourceData(Points& points, const FunctionObject functionObject)
    {
        // Note that PointsType may or may not be "const".
        auto sourceData = points.getSourceDataset<Points>();

        if (sourceData->getId() == points.getId() || points.isFull())
        {
            // In this case, this (points) is itself a source data, or it is a full set.
            // Basically just do sourceData.visitData:
            return privateVisitData<ReturnType>(*sourceData, functionObject);
        }
        else
        {
            // In this case, this (points) is a derived data set, and it is a subset.

            if (sourceData->isFull())
            {
                return sourceData->template visitFromBeginToEnd<ReturnType>(
                    [&points, functionObject](const auto begin, const auto end) -> ReturnType
                    {
                        const auto indexFunction = [](const auto indexIterator)
                        {
                            // Get the index by dereferencing the iterator.
                            return *indexIterator;
                        };

                        // Its source data is a full set, so it is sufficient to use its own (points) indices.
                        return functionObject(mv::makePointDataRangeOfSubset(
                            begin, points.indices, points.getNumDimensions(), indexFunction));
                    });
            }
            else
            {
                // In this case, this (points) is a subset, and its source data is also a subset.

                // Define an index function that translates a derived subset index to a source data index.
                const auto indexFunction = [&sourceData](const auto indexIterator)
                {
                    return sourceData->indices[*indexIterator];
                };

                return sourceData->template visitFromBeginToEnd<ReturnType>(
                    [&points, functionObject, indexFunction](const auto begin, const auto end) -> ReturnType
                    {
                        return functionObject(mv::makePointDataRangeOfSubset(
                            begin, points.indices, points.getNumDimensions(), indexFunction));
                    });
            }
        }
    }

    // When the number of dimensions is changed in setData(), update dimension names and notify core
    void handleNumberDimensionsChanged(std::size_t newNumDimensions);

public:
    Points(QString dataName, bool mayUnderive = true, const QString& guid = "");
    ~Points() override = default;

    using ElementTypeSpecifier = PointData::ElementTypeSpecifier;
    using TiledTileGenerator = PointData::TiledTileGenerator;
    using StorageBackendType = mv::pointdata::storage::StorageBackendType;
    using StorageBackendConfiguration = mv::pointdata::storage::StorageBackendConfiguration;
    using StorageBackendProgressCallback = mv::pointdata::storage::StorageBackendProgressCallback;
    using StorageBackendConversionOptionsPtr = mv::pointdata::storage::StorageBackendConversionOptionsPtr;

    void init() override;

    /// Allows read-only access to the internal point data vector, from its begin to its end.
    /// Similar to C++17 std::visit.
    template <typename ReturnType = void, typename FunctionObject>
    ReturnType constVisitFromBeginToEnd(FunctionObject functionObject) const
    {
        return getRawData<PointData>()->constVisitFromBeginToEnd<ReturnType>(functionObject);
    }

    /// Convenience member function, just calling constVisitFromBeginToEnd.
    template <typename ReturnType = void, typename FunctionObject>
    ReturnType visitFromBeginToEnd(FunctionObject functionObject) const
    {
        return constVisitFromBeginToEnd<ReturnType>(functionObject);
    }

    /// Non-const overload, allowing read + write access to the internal point data vector, from its begin to its end.
    template <typename ReturnType = void, typename FunctionObject>
    ReturnType visitFromBeginToEnd(FunctionObject functionObject)
    {
        return getRawData<PointData>()->visitFromBeginToEnd<ReturnType>(functionObject);
    }


    /* Allows visiting the point data, which is either _all_ data (if this data
     * set is full), or (otherwise) the subset specified by its indices.
    */
    template <typename ReturnType = void, typename FunctionObject>
    ReturnType visitData(FunctionObject functionObject) const
    {
        return privateVisitData<ReturnType>(*this, functionObject);
    }


    /* Non-const overload, allowing write access to the point data.
    */
    template <typename ReturnType = void, typename FunctionObject>
    ReturnType visitData(FunctionObject functionObject)
    {
        return privateVisitData<ReturnType>(*this, functionObject);
    }


    /* Allows visiting the source point data.
    */
    template <typename ReturnType = void, typename FunctionObject>
    ReturnType visitSourceData(const FunctionObject functionObject) const
    {
        return privateVisitSourceData<ReturnType>(*this, functionObject);
    }


    /* Non-const overload, allowing write access to the source point data.
    */
    template <typename ReturnType = void, typename FunctionObject>
    ReturnType visitSourceData(const FunctionObject functionObject)
    {
        return privateVisitSourceData<ReturnType>(*this, functionObject);
    }


    /// Just calls the corresponding member function of its PointData.
    template <typename T>
    void convertData(const T* const data, const std::size_t numPoints, const std::size_t numDimensions)
    {
        getRawData<PointData>()->convertData(data, numPoints, numDimensions);
    }


    /// Just calls the corresponding member function of its PointData.
    template <typename T>
    void convertData(const T& inputDataContainer, const std::size_t numDimensions)
    {
        getRawData<PointData>()->convertData(inputDataContainer, numDimensions);
    }

    template <typename T>
    void setDataElementType()
    {
        getRawData<PointData>()->setElementType<T>();
    }

    /// Just calls the corresponding member function of its PointData.
    template <typename T>
    void setData(const T* const data, const std::size_t numPoints, const std::size_t numDimensions)
    {
        const auto numDimensionsChanged = numDimensions != getRawData<PointData>()->getNumDimensions();

        getRawData<PointData>()->setData(data, numPoints, numDimensions);

        if (numDimensionsChanged)
            handleNumberDimensionsChanged(numDimensions);
    }

    /// Just calls the corresponding member function of its PointData.
    void setData(std::nullptr_t data, std::size_t numPoints, std::size_t numDimensions);

    /// Just calls the corresponding member function of its PointData.
    template <typename T>
    void setData(const std::vector<T>& data, const std::size_t numDimensions)
    {
        const auto numDimensionsChanged = numDimensions != getRawData<PointData>()->getNumDimensions();

        getRawData<PointData>()->setData(data, numDimensions);

        if (numDimensionsChanged)
            handleNumberDimensionsChanged(numDimensions);
    }

    /// Just calls the corresponding member function of its PointData.
    template <typename T>
    void setData(std::vector<T>&& data, const std::size_t numDimensions)
    {
        const auto numDimensionsChanged = numDimensions != getRawData<PointData>()->getNumDimensions();

        getRawData<PointData>()->setData(std::move(data), numDimensions);

        if (numDimensionsChanged)
            handleNumberDimensionsChanged(numDimensions);
    }

    void extractDataForDimension(std::vector<float>& result, const int dimensionIndex) const;

    void extractDataForDimensions(std::vector<mv::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2) const;

    /// Populates the specified result container with the data for the
    /// dimensions specified by the dimension indices.
    /// \note This function does not do any allocation. It assumes that the
    /// specified result container is large enough to store all the data.
    template <typename ResultContainer, typename DimensionIndices>
    void populateDataForDimensions(ResultContainer& resultContainer, const DimensionIndices& dimensionIndices) const
    {
        if (isProxy()) {
            std::size_t offset = 0;

            for (auto& proxyMember : getProxyMembers()) {
                auto points = mv::Dataset<Points>(proxyMember);

                ResultContainer proxyPointsData = {};

                const auto numberOfElements = points->getNumPoints() * dimensionIndices.size();

                proxyPointsData.resize(numberOfElements);

                const auto rawPointData = points->getRawData<PointData>();

                if (points->isFull())
                    rawPointData->populateFullDataForDimensions(proxyPointsData, dimensionIndices);
                else
                    rawPointData->populateDataForDimensions(proxyPointsData, dimensionIndices, points->indices);

                std::copy(proxyPointsData.begin(), proxyPointsData.end(), resultContainer.begin() + offset);

                offset += numberOfElements;
            }
        }
        else {
            const auto rawPointData = getRawData<PointData>();

            if (isFull())
                rawPointData->populateFullDataForDimensions(resultContainer, dimensionIndices);
            else
                rawPointData->populateDataForDimensions(resultContainer, dimensionIndices, indices);
        }
    }

    template <typename ResultContainer, typename DimensionIndices, typename Indices>
    void populateDataForDimensions(ResultContainer& resultContainer, const DimensionIndices& dimensionIndices, const Indices& indices) const
    {
        getRawData<PointData>()->populateDataForDimensions(resultContainer, dimensionIndices, indices);
    }

    std::uint64_t getNumRawPoints() const
    {
        if (isProxy()) {
            return getNumPoints();
        }
        else {
            return getRawData<PointData>()->getNumPoints();
        }
    }

    /**
     * Establish whether a proxy dataset may be created with candidate \p proxyDatasets
     * @param proxyDatasets Candidate proxy datasets
     * @return Boolean indicating whether a proxy dataset may be created with candidate \p proxyDatasets
     */
    bool mayProxy(const mv::Datasets& proxyDatasets) const override;

    std::uint64_t getNumPoints() const
    {
        if (isProxy()) {
            std::uint64_t numberOfPoints = 0;

            for (auto& proxyMember : getProxyMembers())
                numberOfPoints += mv::Dataset<Points>(proxyMember)->getNumPoints();

            return numberOfPoints;
        }
        else {
            if (isFull()) {
                return getRawData<PointData>()->getNumPoints();
            }
            else {
                return static_cast<std::uint64_t>(indices.size());
            }
        }
    }

    std::uint64_t getNumDimensions() const
    {
        if (isProxy()) {
            return mv::Dataset<Points>(getProxyMembers().first())->getNumDimensions();
        }
        else {
            return getRawData<PointData>()->getNumDimensions();
        }
    }

    /** Returns the name of the element type used by this logical dataset. */
    QString getElementTypeName() const;

    /** Returns the physical backend type used by this concrete point dataset. */
    StorageBackendType getStorageBackendType() const;

    /** Returns the capability flags of the physical backend used by this dataset. */
    mv::pointdata::storage::StorageBackendCapabilities getStorageBackendCapabilities() const;

    /** Creates controls for converting to the requested storage backend. */
    std::unique_ptr<mv::gui::StorageBackendConversionAction> createStorageBackendConversionAction(StorageBackendType targetType, QObject* parent = nullptr) const;

    /** Converts this concrete dataset to a storage backend transactionally. */
    void convertToStorageBackend(StorageBackendType type, const StorageBackendConfiguration& configuration = {}, StorageBackendProgressCallback progress = {});

    /** Converts using backend-specific options supplied by a conversion action. */
    void convertToStorageBackend(StorageBackendType type, StorageBackendConversionOptionsPtr options, StorageBackendProgressCallback progress = {});

    /**
     * Applies options owned by the current storage backend of this concrete
     * dataset. Proxy datasets are rejected because their members may use
     * different physical backends and configurations.
     */
    void configureStorageBackend(StorageBackendConversionOptionsPtr options);

    /** Creates read-only generator-backed tiled storage for a concrete, non-proxy dataset. */
    void makeTiledFromGenerator(std::uint64_t rows, std::uint64_t columns, ElementTypeSpecifier elementType, std::size_t rowTileSize, std::size_t columnTileSize, TiledTileGenerator generator);

    /** Returns one logical row as a storage-neutral read-only view, translating subset or proxy indices. */
    PointDataAccessView getRow(std::uint64_t row) const;

    /** Visits a contiguous range of logical rows, translating proxy and subset indices. */
    void visitRows(std::uint64_t firstRow, std::uint64_t rowCount, const PointDataAccessVisitor& visitor) const;

    /** Visits selected logical rows in rowIds order. */
    void visitRows(const std::vector<std::uint64_t>& rowIds, const PointDataAccessVisitor& visitor) const;

    /** Visits a contiguous column range for every logical row. */
    void visitColumns(std::uint64_t firstColumn, std::uint64_t columnCount, const PointDataAccessVisitor& visitor) const;

    /** Visits selected logical columns in columnIds order for the logical dataset. */
    void visitColumns(const std::vector<std::uint64_t>& columnIds, const PointDataAccessVisitor& visitor) const;

    /** Visits a contiguous column range for selected logical rows. */
    void visitColumns(std::uint64_t firstColumn, std::uint64_t columnCount, const std::vector<std::uint64_t>& rowIds, const PointDataAccessVisitor& visitor) const;

    /** Visits a rectangular range of logical rows and columns without full materialization. */
    void visitRange(std::uint64_t firstRow, std::uint64_t rowCount, std::uint64_t firstColumn, std::uint64_t columnCount, const PointDataAccessVisitor& visitor) const;

    /** Returns the element type represented by a type name. */
    static ElementTypeSpecifier elementTypeSpecifier(const QString& typeName)
    {
        return PointData::elementTypeSpecifier(typeName);
    }

    /** Returns the display name for an element type. */
    static QString elementTypeName(const ElementTypeSpecifier elementTypeSpecifier)
    {
        return PointData::elementTypeName(elementTypeSpecifier);
    }

    /** Returns the names of all supported element types. */
    static constexpr auto getElementTypeNames()
    {
        return PointData::getElementTypeNames();
    }

    const std::vector<QString>& getDimensionNames() const;

    void setDimensionNames(const std::vector<QString>& dimNames);

    /**
     * Get amount of data occupied by the raw data
     * @return Size of the raw data in bytes
     */
    std::uint64_t getRawDataSize() const override {
        if (isProxy())
            return 0;
        else {
            return getRawData<PointData>()->getRawDataSize();
        }
    }

    // Returns the value of the element at the specified position in the current
    // data vector, converted to float.
    // Will work fine, even when the internal data element type is not float.
    // However, may not perform well when retrieving a large number of values.
    float getValueAt(std::size_t index) const;

    // Sets the value of the element at the specified position in the current
    // data vector, converted to the internal data element type. 
    // Will work fine, even when the internal data element type is not float.
    // However, may not perform well when setting a large number of values.
    void setValueAt(std::size_t index, float newValue);

    public: // Dense, test implementation
    class Experimental {
        friend class Points;
    public:

        static SparseMatrix<size_t, size_t, float>& getSparseData(Points* points)
        {
            return PointData::Experimental::getSparseData(points->getRawData<PointData>());
        }

        static void setSparseData(Points* points, size_t numRows, size_t numCols, const std::vector<size_t>& rowPointers, const std::vector<size_t>& colIndices, const std::vector<float>& values)
        {
            PointData::Experimental::setSparseData(points->getRawData<PointData>(), numRows, numCols, rowPointers, colIndices, values);
        }

        static void setSparseData(Points* points, size_t numRows, size_t numCols, std::vector<size_t>&& rowPointers, std::vector<size_t>&& colIndices, std::vector<float>&& values)
        {
            PointData::Experimental::setSparseData(points->getRawData<PointData>(), numRows, numCols, std::move(rowPointers), std::move(colIndices), std::move(values));
        }

        static std::vector<float> row(const Points* points, size_t rowIndex)
        {
            return PointData::Experimental::row(points->getRawData<PointData>(), rowIndex);
        }

        static size_t getNumNonZeroElements(const Points* points)
        {
            return PointData::Experimental::getNumNonZeroElements(points->getRawData<PointData>());
        }

        static bool isDense(const Points* points)
        {
            return PointData::Experimental::isDense(points->getRawData<PointData>());
        }

        static void setIsDense(Points* points, bool isDense)
        {
            PointData::Experimental::setIsDense(points->getRawData<PointData>(), isDense);
        }
    };

    /**
     * Get a copy of the dataset
     * @return Smart pointer to copy of dataset
     */
    mv::Dataset<mv::DatasetImpl> copy() const override;

    /**
     * Create subset from the current selection and specify where the subset will be placed in the data hierarchy
     * @param guiName Name of the subset in the GUI
     * @param parentDataSet Smart pointer to parent dataset in the data hierarchy (default is below the set)
     * @param visible Whether the subset will be visible in the UI
     * @return Smart pointer to the created subset
     */
    mv::Dataset<mv::DatasetImpl> createSubsetFromSelection(const QString& guiName, const mv::Dataset<mv::DatasetImpl>& parentDataSet = mv::Dataset<mv::DatasetImpl>(), const bool& visible = true) const override;

    /**
     * Create subset from the current selection and specify where the subset will be placed in the data hierarchy.
     * The subset points computed from the current selection are limited to the ones also contained in the parent data.
     * @param guiName Name of the subset in the GUI
     * @param parentDataSet Smart pointer to parent dataset in the data hierarchy (default is below the set)
     * @param visible Whether the subset will be visible in the UI
     * @return Smart pointer to the created subset
     */
    mv::Dataset<mv::DatasetImpl> createSubsetFromVisibleSelection(const QString& guiName, const mv::Dataset<mv::DatasetImpl>& parentDataSet = mv::Dataset<mv::DatasetImpl>(), const bool& visible = true) const;

    /**
     * Set the proxy member datasets (automatically sets the dataset type to Type::Proxy)
     * @param proxyMembers Proxy member datasets
     */
    void setProxyMembers(const mv::Datasets& proxyMembers) override;

public: // Index transformation
    /**
     * Get the indices over the original source data that this dataset
     * indexes into through being a subset or derived data or a combination.
     * @param globalIndices Resulting vector of global indices into the original raw data
     */
    void getGlobalIndices(std::vector<std::uint32_t>& globalIndices) const;

    /**
     * Passing a vector of global selection indices, returns a vector of booleans
     * describing which indices of this dataset are selected. A locally selected
     * point will have true in its corresponding index, and false if not.
     * @param Vector of global selection indices
     * @param Boolean vector of locally selected points
     */
    void selectedLocalIndices(const std::vector<std::uint32_t>& selectionIndices, std::vector<bool>& selected) const;

    void getLocalSelectionIndices(std::vector<std::uint32_t>& localSelectionIndices) const;


public: // Action getters

    InfoAction& getInfoAction();
    PointsAction& getPointsAction();
    mv::gui::GroupAction& getDimensionsPickerGroupAction();
    DimensionsPickerAction& getDimensionsPickerAction();

public: // Selection

    /**
     * Get selection indices
     * @return Selection indices
     */
    std::vector<std::uint32_t>& getSelectionIndices() override;

    /**
     * Select by indices
     * @param indices Selection indices
     */
    void setSelectionIndices(const std::vector<std::uint32_t>& indices) override;

    /** Determines whether items can be selected */
    bool canSelect() const override;

    /** Determines whether all items can be selected */
    bool canSelectAll() const override;

    /** Determines whether there are any items which can be deselected */
    bool canSelectNone() const override;

    /** Determines whether the item selection can be inverted (more than one) */
    bool canSelectInvert() const override;

    /** Select all items */
    void selectAll() override;

    /** De-select all items */
    void selectNone() override;

    /** Invert item selection */
    void selectInvert() override;

public: // Serialization

    /**
     * Create a workflow that restores this object's state from a variant map.
     *
     * See Serializable::fromVariantMapWorkflow() for the full contract,
     * execution semantics, and implementation requirements.
     *
     * @param variantMap Serialized object state.
     * @return Workflow plan that restores the object state when executed.
     */
    mv::workflow::UniqueWorkflowPlan fromVariantMapWorkflow(QVariantMap variantMap) override;

    /**
     * Create a workflow that serializes this object's state to a variant map.
     *
     * See Serializable::toVariantMapWorkflow() for the full contract,
     * execution semantics, and implementation requirements.
     *
     * @return Workflow plan that serializes the object state when executed.
     */
    mv::workflow::UniqueWorkflowPlan toVariantMapWorkflow() const override;

public: // Linked data

    /**
     * Resolves linked data for the dataset
     * @param force Force update of all linked data (ignores linked data flags)
     */
    void resolveLinkedData(bool force = false) override;

public:

    std::vector<std::uint32_t> indices;

    InfoAction*                 _infoAction;                    /** Non-owning pointer to info action */
    PointsAction*               _pointsAction;                   /** Non-owning pointer to points action */
    mv::gui::GroupAction*       _dimensionsPickerGroupAction;   /** Group action for dimensions picker action */
    DimensionsPickerAction*     _dimensionsPickerAction;        /** Non-owning pointer to dimensions picker action */
    mv::EventListener           _eventListener;                 /** Listen to HDPS events */

    friend class mv::legacy::PointsLegacySerializer;
};

// =============================================================================
// Factory
// =============================================================================

/**
 * @brief Registers and constructs the PointData raw-data plugin.
 *
 * PointDataFactory connects the Points dataset type to the plugin manager and
 * keeps creation, metadata, and serialization discovery separate from the
 * PointData storage implementation. The factory supports the established
 * eager serialized representation while PointData may restore an additive
 * tiled representation when present in a project.
 *
 * @author Thomas Kroes
 */
class PointDataFactory : public mv::plugin::RawDataFactory
{
    Q_INTERFACES(mv::plugin::RawDataFactory mv::plugin::PluginFactory)
        Q_OBJECT
        Q_PLUGIN_METADATA(IID   "studio.manivault.PointData"
                          FILE  "PluginInfo.json")

public:
    PointDataFactory();

    ~PointDataFactory() override = default;

    /**
     * Get the read me markdown file URL
     * @return URL of the read me markdown file
     */
    QUrl getReadmeMarkdownUrl() const override;

    /**
     * Get the URL of the GitHub repository
     * @return URL of the GitHub repository
     */
    QUrl getRepositoryUrl() const override;

    mv::plugin::RawData* produce() override;
};
