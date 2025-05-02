// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "pointdata_export.h"

#include "RawData.h"


#include "LinkedData.h"
#include "PointDataRange.h"
#include "Set.h"
#include "SparseMatrix.h"

#include "PointDataUtil.h"
#include "VariantOfMatrixTypes.h"
#include "MatrixUtils.h"

#include "event/EventListener.h"

//#include <biovault_bfloat16/biovault_bfloat16.h>

#include <QDebug>
#include <QMap>
#include <QString>
#include <QVariant>

#include <array>
#include <cassert>
#include <utility>
#include <variant>
#include <vector>

using namespace mv::plugin;




namespace mv
{
    // From "graphics/Vector2f.h"
    class Vector2f;

    namespace gui {
        class GroupAction;
    }

    using point_size_t = std::uint32_t;
    using dimension_size_t = std::uint32_t;
    using element_size_t = std::uint64_t;
}

// =============================================================================
// Data Type
// =============================================================================

const mv::DataType PointType = mv::DataType(QString("Points"));

class InfoAction;
class DimensionsPickerAction;
class ClusterAction;

// =============================================================================
// Raw Data
// =============================================================================

class POINTDATA_EXPORT PointData : public mv::plugin::RawData
{

    

public:

    

    using VariantOfMatrices = mv_matrix::VariantOfMatrices;

    class StorageConfiguration : protected mv_matrix::Configuration
    {
        friend class PointData;
        static constexpr std::array<const char*, 3> SpecialDataTypeNames = { "Original", "Optimized (lossless)", "Optimized (bfloat16)" };
        enum SpecialDataTypeID { SET_Original, SET_lossless, SET_bfloat16 };

        StorageConfiguration() = delete;
    public:
        StorageConfiguration(const StorageConfiguration  &other) = default;
            
        StorageConfiguration(const std::string &storageTypeName, const std::string &dataTypeName)
            : mv_matrix::Configuration(storageTypeName, dataTypeName)
        {

        }

        StorageConfiguration(const QString &storageTypeName, const QString &dataTypeName)
            : mv_matrix::Configuration(storageTypeName.toStdString(), dataTypeName.toStdString())
        {

        }

        StorageConfiguration(const std::string &dataTypeName)
            : mv_matrix::Configuration(dataTypeName)
        {

        }

        StorageConfiguration(const char * dataTypeName)
            : mv_matrix::Configuration(dataTypeName)
        {

        }

        template<typename T>
        StorageConfiguration(const std::string &storageTypeName, T )
            : mv_matrix::Configuration::Configuration(storageTypeName, T())
        {
            static_assert(std::numeric_limits<T>::is_specialized);
        }

        template<typename T>
        StorageConfiguration(std::nullptr_t a, T)
            : mv_matrix::Configuration::Configuration(a, T())
        {
            static_assert(std::numeric_limits<T>::is_specialized);
        }
       

        template<typename T>
        void setDataType()
        {
            setDataTypeTo<T>();
        }


        QString qStorageTypeName() const
        {
            return QString::fromStdString(storageType());
        }

        QString qDataTypeName() const
        {
            return QString::fromStdString(dataType());
        }

        bool isSpecialDataType() const
        {
            auto found = std::find(SpecialDataTypeNames.cbegin(), SpecialDataTypeNames.cend(), dataType());
            return (found != SpecialDataTypeNames.cend());
        }

        bool allowBfloat16Optimization() const
        {
            return (dataType() == SpecialDataTypeNames[SET_bfloat16]);
        }

        bool isOriginal() const
        {
            return (dataType() == SpecialDataTypeNames[SET_Original]);
        }

        bool isLosslessOptimized() const
        {
            return (dataType() == SpecialDataTypeNames[SET_lossless]);
        }

        bool isOptimized() const
        {
            return (allowBfloat16Optimization() || isLosslessOptimized());
        }

        void optimizeDataType()
        {
            Configuration::setDataType(SpecialDataTypeNames[SET_lossless]);
        }

        static QStringList getListOfSpecialDataTypeNames()
        {
            return QStringList(SpecialDataTypeNames.cbegin(), SpecialDataTypeNames.cend());
        }
    };

    static constexpr std::size_t NumberOfElementTypes = VariantOfMatrices::NumberOfElementTypes;

    static constexpr auto ElementTypeNames()
    {
        return VariantOfMatrices::getElementTypeNames();
    }

    static constexpr auto MatrixTypeNames()
    {
        return VariantOfMatrices::getMatrixTypeNames();
    }

    __declspec(deprecated) static constexpr auto getElementTypeNames()
    {
        return VariantOfMatrices::getElementTypeNames();
    }
    
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

private:

    template <typename T>
    static constexpr ElementTypeSpecifier getElementTypeSpecifier()
    {
        constexpr auto index = VariantOfMatrices::getIndexOfType<mv_matrix::DenseMatrix<T>>();
        return static_cast<ElementTypeSpecifier>(index);
    }

    template <typename T>
    __declspec(deprecated)  const std::vector<T>& getConstVector() const
    {
        // This function should only be used to access the currently selected vector.
        static_assert(std::holds_alternative<mv_matrix::DenseMatrix<T>>(_variantOfMatrices));
        return getConstVector<T>(std::get<mv_matrix::DenseMatrix<T>>(_variantOfMatrices));
    }

    template <typename T>
    __declspec(deprecated) const std::vector<T>& getVector() const
    {
        return getConstVector<T>();
    }

    template <typename T>
    __declspec(deprecated) std::vector<T>& getVector()
    {
        return const_cast<std::vector<T>&>(getConstVector<T>());
    }

    /// Returns the size of the std::vector currently held by _variantOfVectors.
    __declspec(deprecated) std::size_t getSizeOfVector() const
    {
        const std::string mesg = __FUNCSIG__ + std::string(" is not implemented");
        throw std::exception(mesg.c_str());
        return 0;
    }

    /// Resizes the std::vector currently held by _variantOfVectors.
    __declspec(deprecated) void resizeVector(const std::size_t newSize)
    {
        const std::string mesg = __FUNCSIG__ + std::string(" is not implemented");
        throw std::exception(mesg.c_str());
       
    }

   

    ElementTypeSpecifier getElementTypeSpecifier() const
    {
        return static_cast<ElementTypeSpecifier>(_variantOfMatrices.getElementTypeIndex());
    }

    /// Resizes the currently held data vector to the specified
    /// number of elements, and converts the elements of the specified data
    /// to the internal data element type, by static_cast. 
    template <typename T>
    void convertData(const T* const data, const std::size_t numberOfElements)
    {
        
        std::visit([data, numberOfElements](auto& mat)
            {
                mat.convertData(data, numberOfElements);
            },
            _variantOfMatrices);
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
    using ElementTypeAt = VariantOfMatrices::ElementTypeAt<N>;

    PointData(PluginFactory* factory) : RawData(factory, PointType) { }
    ~PointData(void) override;

    void init() override;

    mv::Dataset<mv::DatasetImpl> createDataSet(const QString& guid = "") const override;

    mv::point_size_t getNumPoints() const;

    mv::dimension_size_t getNumDimensions() const;

    mv::element_size_t getNumberOfElements() const;

    /**
     * Get amount of data occupied by the raw data
     * @return Size of the raw data in bytes
     */
    std::uint64_t getRawDataSize() const override;

    /**
     *Returns void pointer to the underlying array serving as element storage.
     */
    void* getDataVoidPtr();
    const void* getDataConstVoidPtr() const;

    // Similar to C++17 std::visit.
    template <typename ReturnType = void, typename FunctionObject>
    ReturnType constVisitFromBeginToEnd(FunctionObject functionObject) const
    {
        
        return std::visit([functionObject](const auto& matrix) -> ReturnType
            {
                return matrix.constVisitFromBeginToEnd<ReturnType>(functionObject);
            },
            _variantOfMatrices);
    }

    // Similar to C++17 std::visit.
    template <typename ReturnType = void, typename FunctionObject>
    ReturnType visitFromBeginToEnd(FunctionObject functionObject)
    {
        
        return std::visit([functionObject](auto& matrix) -> ReturnType
            {
                return matrix.visitFromBeginToEnd<ReturnType>(functionObject);
            },
            _variantOfMatrices);
    }

    // New Dense and Sparse Matrix visit functions

    /**
     * Visit elements in a sparse manner (so ignore zero values).
     *
     * @param functionObject  Function or lambda to apply to each data element in the matrix
     * @param task pointer to DatasetTask to track progress.
     */
    template <typename FunctionObject>
    void sparseVisit(FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
    {
        return std::visit([functionObject,task](auto& matrix)
            {
                matrix.sparseVisitElements(functionObject, task);
            },
            _variantOfMatrices);
    }

    /**
    * Visit elements in a dense manner (so also visit 0 values).
    *
    * @param functionObject  Function or lambda to apply to each data element in the matrix
    * @param task pointer to DatasetTask to track progress.
    */
    template <typename FunctionObject>
    void denseVisit(FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
    {
        return std::visit([functionObject, task](auto& matrix)
            {
                matrix.denseVisitElements(functionObject, task);
            },
            _variantOfMatrices);
    }

    /**
    * Parallel Visit elements row-wise in a sparse manner
    *
    * @param functionObject  Function or lambda to apply to each data element in the matrix
    * @param task pointer to DatasetTask to track progress.
    */
    template <typename FunctionObject>
    void sparseParRowVisit(FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
    {
        return std::visit([functionObject, task](auto& matrix)
            {
                matrix.rowParallelSparseVisitElements(functionObject, task);
            },
            _variantOfMatrices);
    }

    /**
   * Parallel Visit elements row-wise in a dense manner
   *
   * @param functionObject  Function or lambda to apply to each data element in the matrix
   * @param task pointer to DatasetTask to track progress.
   */
    template <typename FunctionObject>
    void denseParRowVisit(FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
    {
        return std::visit([functionObject, task](auto& matrix)
            {
                matrix.rowParallelDenseVisitElements(functionObject, task);
            },
            _variantOfMatrices);
    }

    /**
    * Parallel Visit elements column-wise in a sparse manner
    *
    * @param functionObject  Function or lambda to apply to each data element in the matrix
    * @param task pointer to DatasetTask to track progress.
    */
    template <typename FunctionObject>
    void sparseParColVisit(FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
    {
        return std::visit([functionObject, task](auto& matrix)
            {
                matrix.columnParallelSparseVisitElements(functionObject, task);
            },
            _variantOfMatrices);
    }

    /**
    * Parallel Visit elements column-wise in a dense manner
    *
    * @param functionObject  Function or lambda to apply to each data element in the matrix
    * @param task pointer to DatasetTask to track progress.
    */
    template <typename FunctionObject>
    void denseParColVisit(FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
    {
        return std::visit([functionObject, task](auto& matrix)
            {
                matrix.columnParallelDenseVisitElements(functionObject, task);
            },
            _variantOfMatrices);
    }

    /**
    * Parallel Visit elements row-wise in a sparse manner
    *
    * @param rows The range of rows to visit
    * @param functionObject  Function or lambda to apply to each data element in the matrix
    * @param task pointer to DatasetTask to track progress.
    */
    template <typename RowRange, typename FunctionObject>
    void sparseParRowVisit(const RowRange& rows, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
    {
        return std::visit([rows, functionObject, task](auto& matrix)
            {
                matrix.rowParallelSparseVisitElements(rows, functionObject, task);
            },
            _variantOfMatrices);
    }

    /**
    * Parallel Visit elements row-wise in a dense manner
    *
    * @param rows The range of rows to visit
    * @param functionObject  Function or lambda to apply to each data element in the matrix
    * @param task pointer to DatasetTask to track progress.
    */
    template <typename RowRange, typename FunctionObject>
    void denseParRowVisit(const RowRange& rows, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
    {
        return std::visit([rows, functionObject, task](auto& matrix)
            {
                matrix.rowParallelDenseVisitElements(rows, functionObject, task);
            }, _variantOfMatrices);
    }

    /**
    * Parallel Visit elements column-wise in a sparse manner
    *
    * @param rows The range of rows to visit
    * @param functionObject  Function or lambda to apply to each data element in the matrix
    * @param task pointer to DatasetTask to track progress.
    */
    template <typename RowRange, typename FunctionObject>
    void sparseParColVisit(const RowRange& rows, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
    {
        return std::visit([rows, functionObject, task](auto& matrix)
            {
                matrix.columnParallelSparseVisitElements(rows, functionObject, task);
            },
            _variantOfMatrices);
    }

    /**
   * Parallel Visit elements column-wise in a dense manner
   *
   * @param rows The range of rows to visit
   * @param functionObject  Function or lambda to apply to each data element in the matrix
   * @param task pointer to DatasetTask to track progress.
   */
    template <typename RowRange, typename FunctionObject>
    void denseParColVisit(const RowRange& rows, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
    {
        return std::visit([rows, functionObject, task](auto& matrix)
            {
                matrix.columnParallelDenseVisitElements(rows, functionObject, task);
            },
            _variantOfMatrices);
    }

    /**
    * Parallel Visit elements row-wise in a sparse manner
    *
    * @param rows The range of rows to visit
    * @param rows The range of columns to visit
    * @param functionObject  Function or lambda to apply to each data element in the matrix
    * @param task pointer to DatasetTask to track progress.
    */
    template <typename RowRange, typename ColumnRange, typename FunctionObject>
    void sparseParRowVisit(const RowRange& rows, const ColumnRange& columns, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
    {
        return std::visit([rows, columns, functionObject, task](auto& matrix)
            {
                matrix.rowParallelSparseVisitElements(rows, columns, functionObject, task);
            },
            _variantOfMatrices);
    }

    /**
    * Parallel Visit elements row-wise in a dense manner
    *
    * @param rows The range of rows to visit
    * @param rows The range of columns to visit
    * @param functionObject  Function or lambda to apply to each data element in the matrix
    * @param task pointer to DatasetTask to track progress.
    */
    template <typename RowRange, typename ColumnRange, typename FunctionObject>
    void denseParRowVisit(const RowRange& rows, const ColumnRange& columns, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
    {
        return std::visit([rows, columns, functionObject, task](auto& matrix)
            {
                matrix.rowParallelDenseVisitElements(rows, columns, functionObject, task);
            },
            _variantOfMatrices);
    }

    /**
    * Parallel Visit elements column-wise in a sparse manner
    *
    * @param rows The range of rows to visit
    * @param rows The range of columns to visit
    * @param functionObject  Function or lambda to apply to each data element in the matrix
    * @param task pointer to DatasetTask to track progress.
    */
    template <typename RowRange, typename ColumnRange, typename FunctionObject>
    void sparseParColVisit(const RowRange& rows, const ColumnRange& columns, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
    {
        return std::visit([rows, columns, functionObject, task](auto& matrix)
            {
                matrix.columnParallelSparseVisitElements(rows, columns, functionObject, task);
            },
            _variantOfMatrices);
    }

    /**
    * Parallel Visit elements column-wise in a dense manner
    *
    * @param rows The range of rows to visit
    * @param rows The range of columns to visit
    * @param functionObject  Function or lambda to apply to each data element in the matrix
    * @param task pointer to DatasetTask to track progress.
    */
    template <typename RowRange, typename ColumnRange, typename FunctionObject>
    void denseParColVisit(const RowRange& rows, const ColumnRange& columns, FunctionObject functionObject, mv::DatasetTask* task = nullptr) const
    {
        return std::visit([rows, columns, functionObject, task](auto& matrix)
            {
                matrix.columnParallelDenseVisitElements(rows, columns, functionObject, task);
            },
            _variantOfMatrices);
    }


    void extractFullDataForDimension(std::vector<float>& result, const int dimensionIndex, mv::DatasetTask* task = nullptr) const;
    void extractFullDataForDimensions(std::vector<mv::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2, mv::DatasetTask* task = nullptr) const;
    void extractDataForDimensions(std::vector<mv::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2, const std::vector<unsigned int>& indices, mv::DatasetTask* task = nullptr) const;
    
    template <typename ResultContainer, typename DimensionIndices>
    void populateFullDataForDimensions(ResultContainer& resultContainer, const DimensionIndices& dimensionIndices, mv::DatasetTask* task = nullptr) const
    {
        CheckDimensionIndices(dimensionIndices);
        std::visit([&resultContainer, &dimensionIndices, task](const auto& matrix)
            {
                matrix.populateFullDataForDimensions(resultContainer, dimensionIndices, task);
            },
            _variantOfMatrices);
    }

    template <typename ResultContainer, typename DimensionIndices, typename Indices>
    void populateDataForDimensions(ResultContainer& resultContainer, const DimensionIndices& dimensionIndices, const Indices& indices, mv::DatasetTask* task = nullptr) const
    {
        CheckDimensionIndices(dimensionIndices);
        std::visit([&resultContainer, &dimensionIndices, &indices, task](const auto& matrix)
            {
                matrix.populateDataForDimensions(resultContainer, dimensionIndices, indices, task);
            },
            _variantOfMatrices);
    }

    const std::vector<QString>& getDimensionNames() const;

    /// Returns the number of types, supported as element type of the internal data storage. 
    static constexpr auto getNumberOfSupportedElementTypes()
    {
        return ElementTypeNames().size();
    }

   

    template <typename T>
    void setElementType()
    {
        _variantOfMatrices.setMatrixType<mv_matrix::DenseMatrix<T>>();
    }

    bool setStorageConfiguration(const StorageConfiguration &config)
    {
        return _variantOfMatrices.setConfiguration(config);
    }

    /// Converts the specified data to the internal data, using static_cast for
    /// each data element. Sets the number of dimensions as specified. Ensures
    /// that the size of the internal data buffer corresponds to the number of
    /// points.
    /// \note This function does not affect the selected internal data type.
    template <typename T>
    void convertData(const T* const data, const mv::point_size_t numPoints, const  mv::dimension_size_t numDimensions)
    {
        std::visit([this, data, numPoints, numDimensions](auto& matrix)
            {
                matrix.convertData(data, numPoints * numDimensions);
            }, _variantOfMatrices);
        _numDimensions = static_cast<std::uint32_t>(numDimensions);
    }

    /// Converts the specified data to the internal data, using static_cast for each data element.
    /// Convenience overload, allowing an std::vector or an std::array as
    /// input data container.
    template <typename T>
    void convertData(const T& inputDataContainer, const  mv::dimension_size_t numDimensions)
    {
        std::visit([this, &inputDataContainer, numDimensions](auto& matrix)
            {
                matrix.convertData(inputDataContainer.data(), inputDataContainer.size());
            }, _variantOfMatrices);
        _numDimensions = static_cast<std::uint32_t>(numDimensions);
    }

    /// Copies the specified data into the internal data, sets the number of
    /// dimensions as specified, and sets the selected internal data type
    /// according to the specified data type T.
    template <typename T>
    void setData(const T* const data, const mv::point_size_t numPoints, const mv::dimension_size_t numDimensions)
    {
        _variantOfMatrices.setMatrixType<mv_matrix::DenseMatrix<T>>();
        _numRows = static_cast<std::uint64_t>(numPoints);
        _numDimensions = static_cast<std::uint32_t>(numDimensions);

        std::visit([&data, numPoints, numDimensions](auto& matrix)
            {
                matrix.setData(data, numPoints, numDimensions);
            }, _variantOfMatrices);
    }


    /// Convenience overload to allow clearing the data by setData(nullptr, 0, numDimensions). 
    void setData(std::nullptr_t data, std::size_t numPoints, std::size_t numDimensions);

    
    

private:

    template <typename T, typename U>
    bool tryToSetStorageTypeHead(const std::string& matrixType, const T minVal, const T maxVal)
    {
        if constexpr (sizeof(U) <= sizeof(T))
            if (mv_matrix::in_range<U>(minVal, maxVal))
                if (_variantOfMatrices.setConfiguration(mv_matrix::Configuration(matrixType, U() )))
                return true;
        return false;
    }

    template <typename T, typename Head, typename... Tail>
    bool tryToSetStorageType(const std::string& matrixType, const T minVal, const T maxVal)
    {
        if (tryToSetStorageTypeHead<T, Head>(matrixType, minVal, maxVal))
            return true;
        else if constexpr (sizeof...(Tail))
            return tryToSetStorageType<T, Tail...>(matrixType, minVal, maxVal);
        return false;
    }


    template <typename T>
    bool setDataStorageType(StorageConfiguration storageConfiguration, const std::vector<T>& values)
    {
        bool storageTypeSet = false;

        if (!storageConfiguration.isSpecialDataType())
        {
            storageTypeSet = setStorageConfiguration(storageConfiguration);
            if (!storageTypeSet)
            {
                // try to find an alternative
                storageConfiguration.optimizeDataType();
                return setDataStorageType(storageConfiguration, values);
            }
        }
        else
        {
            if (storageConfiguration.isOriginal())
            {
                storageConfiguration.setDataType<T>();
                storageTypeSet = setStorageConfiguration(storageConfiguration);
                if (!storageTypeSet)
                {
                    // try to find an alternative
                    storageConfiguration.optimizeDataType();
                    return setDataStorageType(storageConfiguration, values);
                }
            }
            else // Find optimal elementType to store the data 
            {
                const bool allowBfloat16 = storageConfiguration.allowBfloat16Optimization();
                auto sizeOfT = sizeof(T);
                auto minmax_pair = std::minmax_element(values.cbegin(), values.cend());
                T minVal = *(minmax_pair.first);
                T maxVal = *(minmax_pair.second);

                const std::string storageType = storageConfiguration.storageType();
                if (mv::pointutils::is_integer(minVal) && mv::pointutils::is_integer(maxVal) && mv::pointutils::contains_only_integers(values))
                {
                    if (*(minmax_pair.first) < 0)
                    {
                        if (allowBfloat16)
                            storageTypeSet = tryToSetStorageType<T, std::int8_t, std::int16_t, biovault::bfloat16_t, std::int32_t>(storageType, minVal, maxVal);
                        else
                            storageTypeSet = tryToSetStorageType<T, std::int8_t, std::int16_t, std::int32_t>(storageType, minVal, maxVal);
                    }
                    else
                    {
                        if (allowBfloat16)
                            storageTypeSet = tryToSetStorageType<T, std::uint8_t, std::uint16_t, biovault::bfloat16_t, std::uint32_t, float, std::uint64_t, double>(storageType, minVal, maxVal);
                        else
                            storageTypeSet = tryToSetStorageType<T, std::uint8_t, std::uint16_t, std::uint32_t, float, std::uint64_t, double>(storageType, minVal, maxVal);
                    }
                }

                if (!storageTypeSet)
                {
                    if (allowBfloat16)
                        storageTypeSet = tryToSetStorageType<T, biovault::bfloat16_t, float, double>(storageType, minVal, maxVal);
                    else
                        storageTypeSet = tryToSetStorageType<T, float, double>(storageType, minVal, maxVal);
                }
            }
        }
        
        return storageTypeSet;
    }
public:

    

    template <typename T>
    bool setDataAs(const StorageConfiguration &storageConfiguration, std::vector<T>& data, const std::size_t numDimensions)
    {
        bool storageTypeSet = setDataStorageType(storageConfiguration, data);

        if (storageTypeSet)
        {
            std::visit([&data, numDimensions,this](auto& matrix) {

                matrix.setData(data, numDimensions);
                _numDimensions = matrix.columns();
                _numRows = matrix.rows();

                }, _variantOfMatrices);
        }

        return storageTypeSet;
    }

   

    template <typename T>
    __declspec(deprecated) void setData(std::vector<T> data, const mv::dimension_size_t numDimensions)
    {
        setDataAs(data, numDimensions);
    }

    template<typename RowSizeType, typename DimensionSizeType, typename RowType, typename ColumnType, typename ValueType>
    bool setDataAs(const StorageConfiguration &storageConfiguration, RowSizeType numRows, DimensionSizeType numDimensions, std::vector<RowType>& rowPointers, std::vector<ColumnType>& colIndices, std::vector<ValueType>& values)
    {
        bool storageTypeSet = setDataStorageType(storageConfiguration, values);

        if (storageTypeSet)
        {
            std::visit([&rowPointers, &colIndices, &values, numRows, numDimensions,this](auto& matrix) {

                matrix.setData(numRows, numDimensions, rowPointers, colIndices, values);
                _numDimensions = matrix.columns();
                _numRows = matrix.rows();
                }, _variantOfMatrices);
        }

        return storageTypeSet;
    }

    template<typename RowSizeType, typename DimensionSizeType, typename RowType, typename ColumnType, typename ValueType>
    bool setDataAs(const QString& matrixType, RowSizeType numRows, DimensionSizeType numDimensions, std::vector<RowType>& rowPointers, std::vector<ColumnType>& colIndices, std::vector<ValueType>& values)
    {
        StorageConfiguration config(matrixType, ValueType());
        return setDataAs(config, data, numDimensions);
    }

    template <typename T>
    bool setDataAs(const QString& matrixType, std::vector<T>& data, const std::size_t numDimensions)
    {
        StorageConfiguration config(matrixType, T());
        return setDataAs(config, data, numDimensions);
    }

    template <typename T>
    bool setDataAs(std::vector<T>& data, const std::size_t numDimensions)
    {
        StorageConfiguration config(nullptr, T());
        return setDataAs(config, data, numDimensions);
    }

    static QMap<QString, QStringList> getSupportedStorageTypes();
    

    void setDimensionNames(const std::vector<QString>& dimNames);

    // Returns the value of the element at the specified position in the current
    // data vector, converted to float.
    // Will work fine, even when the internal data element type is not float.
    // However, may not perform well when retrieving a large number of values.
    __declspec(deprecated) float getValueAt(mv::element_size_t index) const;

    // Sets the value of the element at the specified position in the current
    // data vector, converted to the internal data element type. 
    // Will work fine, even when the internal data element type is not float.
    // However, may not perform well when setting a large number of values.
    __declspec(deprecated) void setValueAt(mv::element_size_t index, float newValue);

    // Returns the value of the element at the specified position in the current
    // data vector, converted to float.
    // Will work fine, even when the internal data element type is not float.
    // However, may not perform well when retrieving a large number of values.
    float getValueAt(mv::point_size_t point_idx, mv::dimension_size_t dim_idx) const;

    // Sets the value of the element at the specified position in the current
    // data vector, converted to the internal data element type. 
    // Will work fine, even when the internal data element type is not float.
    // However, may not perform well when setting a large number of values.
    void setValueAt(mv::point_size_t point_idx, mv::dimension_size_t dim_idx, float newValue);

  


public: // Serialization
    /**
     * Load point data from variant map
     * @param Variant map representation of the point data
     */
    void fromVariantMap(const QVariantMap& variantMap) final;

    /**
     * Save point data to variant map
     * @return Variant map representation of the point data
     */
    QVariantMap toVariantMap() const final;

    

private:
    VariantOfMatrices _variantOfMatrices;

    /** Number of features of each data point */
    mv::dimension_size_t _numDimensions = 1;

    std::vector<QString> _dimNames;


    mv::point_size_t _numRows = 0;

    
    bool _isDense = true;

    
};

// =============================================================================
// Point Set
// =============================================================================

class POINTDATA_EXPORT Points : public mv::DatasetImpl
{

    mv::DatasetTask* getDatasetTaskPtr(const QString& description) const;

    /* Private helper function for visitData. Helps to reduces duplicate
    * code between const and non-const overloads of visitData.
    */
    template <typename ReturnType = void, typename FunctionObject>
    static ReturnType privateVisitData(Points& points, const FunctionObject functionObject)
    {
        assert(false); // still need to check this for sparse matrices

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
        assert(false); // still need to check this for sparse matrices

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

public:

    using StorageConfiguration = PointData::StorageConfiguration;

    Points(QString dataName, bool mayUnderive = true, const QString& guid = "");
    ~Points() override;

    void init() override;

    /// Allows read-only access to the internal point data vector, from its begin to its end.
    /// Similar to C++17 std::visit.
    template <typename ReturnType = void, typename FunctionObject>
    __declspec(deprecated) ReturnType constVisitFromBeginToEnd(FunctionObject functionObject) const
    {
        return getRawData<PointData>()->constVisitFromBeginToEnd<ReturnType>(functionObject);
    }

    /// Convenience member function, just calling constVisitFromBeginToEnd.
    template <typename ReturnType = void, typename FunctionObject>
    __declspec(deprecated) ReturnType visitFromBeginToEnd(FunctionObject functionObject) const
    {
        return constVisitFromBeginToEnd<ReturnType>(functionObject);
    }

    /// Non-const overload, allowing read + write access to the internal point data vector, from its begin to its end.
    template <typename ReturnType = void, typename FunctionObject>
    __declspec(deprecated) ReturnType visitFromBeginToEnd(FunctionObject functionObject)
    {
        return getRawData<PointData>()->visitFromBeginToEnd<ReturnType>(functionObject);
    }

    template <typename FunctionObject>
    void sparseVisit(FunctionObject functionObject, const QString& taskDescription = QString()) const
    {
        getRawData<PointData>()->sparseVisitElements(functionObject, getDatasetTaskPtr(taskDescription));
    }

    template <typename FunctionObject>
    void denseVisit(FunctionObject functionObject, const QString& taskDescription = QString()) const
    {
        getRawData<PointData>()->denseVisitElements(functionObject, getDatasetTaskPtr(taskDescription));
    }

    template <typename FunctionObject>
    void sparseParRowVisit(FunctionObject functionObject, const QString& taskDescription = QString()) const
    {
        getRawData<PointData>()->sparseParRowVisit(functionObject, getDatasetTaskPtr(taskDescription));
    }

    template <typename FunctionObject>
    void denseParRowVisit(FunctionObject functionObject, const QString& taskDescription = QString()) const
    {
        getRawData<PointData>()->denseParRowVisit(functionObject, getDatasetTaskPtr(taskDescription));
    }

    template <typename FunctionObject>
    void sparseParColVisit(FunctionObject functionObject, const QString& taskDescription = QString()) const
    {
        getRawData<PointData>()->sparseParColVisit(functionObject, getDatasetTaskPtr(taskDescription));
    }

    template <typename FunctionObject>
    void denseParColVisit(FunctionObject functionObject, const QString& taskDescription = QString()) const
    {
        getRawData<PointData>()->denseParColVisit(functionObject, getDatasetTaskPtr(taskDescription));
    }

    template <typename RowRange, typename FunctionObject>
    void sparseParRowVisit(const RowRange& rows, FunctionObject functionObject, const QString& taskDescription = QString()) const
    {
        getRawData<PointData>()->sparseParRowVisit(rows, functionObject, getDatasetTaskPtr(taskDescription));
    }

    template <typename RowRange, typename FunctionObject>
    void denseParRowVisit(const RowRange& rows, FunctionObject functionObject, const QString& taskDescription = QString()) const
    {
        getRawData<PointData>()->denseParRowVisit(rows, functionObject, getDatasetTaskPtr(taskDescription));
    }

    template <typename RowRange, typename FunctionObject>
    void sparseParColVisit(const RowRange& rows, FunctionObject functionObject, const QString& taskDescription = QString()) const
    {
        getRawData<PointData>()->sparseParColVisit(rows, functionObject, getDatasetTaskPtr(taskDescription));
    }

    template <typename RowRange, typename FunctionObject>
    void denseParColVisit(const RowRange& rows, FunctionObject functionObject, const QString& taskDescription = QString()) const
    {
        getRawData<PointData>()->denseParColVisit(rows, functionObject, getDatasetTaskPtr(taskDescription));
    }

    template <typename RowRange, typename ColumnRange, typename FunctionObject>
    void sparseParRowVisit(const RowRange& rows, const ColumnRange& columns, FunctionObject functionObject, const QString& taskDescription = QString()) const
    {
        getRawData<PointData>()->sparseParRowVisit(rows, columns, functionObject, getDatasetTaskPtr(taskDescription));
    }

    template <typename RowRange, typename ColumnRange, typename FunctionObject>
    void denseParRowVisit(const RowRange& rows, const ColumnRange& columns, FunctionObject functionObject, const QString& taskDescription = QString()) const
    {
        getRawData<PointData>()->denseParRowVisit(rows, columns, functionObject, getDatasetTaskPtr(taskDescription));
    }

    template <typename RowRange, typename ColumnRange, typename FunctionObject>
    void sparseParColVisit(const RowRange& rows, const ColumnRange& columns, FunctionObject functionObject, const QString& taskDescription = QString()) const
    {
        getRawData<PointData>()->sparseParColVisit(rows, columns, functionObject, getDatasetTaskPtr(taskDescription));
    }

    template <typename RowRange, typename ColumnRange, typename FunctionObject>
    void denseParColVisit(const RowRange& rows, const ColumnRange& columns, FunctionObject functionObject, const QString& taskDescription = QString()) const
    {
        getRawData<PointData>()->denseParColVisit(rows, columns, functionObject, getDatasetTaskPtr(taskDescription));
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
        const auto notifyDimensionsChanged = numDimensions != getRawData<PointData>()->getNumDimensions();

        getRawData<PointData>()->setData(data, numPoints, numDimensions);

        if (notifyDimensionsChanged)
            mv::events().notifyDatasetDataDimensionsChanged(this);
    }

    /// Just calls the corresponding member function of its PointData.
    void setData(std::nullptr_t data, std::size_t numPoints, std::size_t numDimensions);

    /// Just calls the corresponding member function of its PointData.
    template <typename T>
    void setDataAs(std::vector<T>& data, const std::size_t numDimensions)
    {
        const auto notifyDimensionsChanged = numDimensions != getRawData<PointData>()->getNumDimensions();

        bool result = getRawData<PointData>()->setDataAs(data, numDimensions);

        if (notifyDimensionsChanged)
            mv::events().notifyDatasetDataDimensionsChanged(this);

        return result;
    }

    template <typename T>
    __declspec(deprecated) void setData(std::vector<T> data, const std::size_t numDimensions)
    {
        const auto notifyDimensionsChanged = numDimensions != getRawData<PointData>()->getNumDimensions();

       getRawData<PointData>()->setDataAs(data, numDimensions);

        if (notifyDimensionsChanged)
            mv::events().notifyDatasetDataDimensionsChanged(this);

        
    }

    

    /// Just calls the corresponding member function of its PointData.
    

    template <typename T>
    bool setDataAs(const StorageConfiguration& config, std::vector<T>& data, const std::size_t numDimensions)
    {
        const auto notifyDimensionsChanged = numDimensions != getRawData<PointData>()->getNumDimensions();

        bool result = getRawData<PointData>()->setDataAs(config, data, numDimensions);

        if (notifyDimensionsChanged)
            mv::events().notifyDatasetDataDimensionsChanged(this);

        return result;
    }

    template<typename RowSizeType, typename DimensionSizeType, typename RowType, typename ColumnType, typename ValueType>
    bool setDataAs(StorageConfiguration storageConfiguration, RowSizeType numRows, DimensionSizeType numDimensions, std::vector<RowType>& rowPointers, std::vector<ColumnType>& colIndices, std::vector<ValueType>& values)
    {
        const auto notifyDimensionsChanged = numDimensions != getRawData<PointData>()->getNumDimensions();

        bool result = getRawData<PointData>()->setDataAs(storageConfiguration, numRows, numDimensions, rowPointers, colIndices, values);

        if (notifyDimensionsChanged)
            mv::events().notifyDatasetDataDimensionsChanged(this);

        return result;
    }

    template<typename RowSizeType, typename DimensionSizeType, typename RowType, typename ColumnType, typename ValueType>
    bool setDataAs(const QString& matrixType, RowSizeType numRows, DimensionSizeType numDimensions, std::vector<RowType>& rowPointers, std::vector<ColumnType>& colIndices, std::vector<ValueType>& values)
    {
        const auto notifyDimensionsChanged = numDimensions != getRawData<PointData>()->getNumDimensions();

        bool result = getRawData<PointData>()->setDataAs(matrixType, numRows, numDimensions, rowPointers, colIndices, values);

        if (notifyDimensionsChanged)
            mv::events().notifyDatasetDataDimensionsChanged(this);

        return result;
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

    unsigned int getNumRawPoints() const
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

    unsigned int getNumPoints() const
    {
        if (isProxy()) {
            auto numberOfPoints = 0;

            for (auto& proxyMember : getProxyMembers())
                numberOfPoints += mv::Dataset<Points>(proxyMember)->getNumPoints();

            return numberOfPoints;
        }
        else {
            if (isFull()) return getRawData<PointData>()->getNumPoints();
                else return static_cast<std::uint32_t>(indices.size());
        }
    }

    unsigned int getNumDimensions() const
    {
        if (isProxy()) {
            return mv::Dataset<Points>(getProxyMembers().first())->getNumDimensions();
        }
        else {
            return getRawData<PointData>()->getNumDimensions();
        }
    }

    static QMap<QString, QStringList> getSupportedStorageTypes();

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
    __declspec(deprecated) float getValueAt(std::size_t index) const;

    // Sets the value of the element at the specified position in the current
    // data vector, converted to the internal data element type. 
    // Will work fine, even when the internal data element type is not float.
    // However, may not perform well when setting a large number of values.
    __declspec(deprecated) void setValueAt(std::size_t index, float newValue);

    // Returns the value of the element at the specified position in the current
    // data vector, converted to float.
    // Will work fine, even when the internal data element type is not float.
    // However, may not perform well when retrieving a large number of values.
    float getValueAt(mv::point_size_t row, mv::dimension_size_t column) const;

    // Sets the value of the element at the specified position in the current
    // data vector, converted to the internal data element type. 
    // Will work fine, even when the internal data element type is not float.
    // However, may not perform well when setting a large number of values.
    void setValueAt(mv::point_size_t row, mv::dimension_size_t column, float newValue);

   
    bool setStorageConfiguration(const StorageConfiguration& config);

    public: // Dense, test implementation
    

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
    void getGlobalIndices(std::vector<unsigned int>& globalIndices) const;

    /**
     * Passing a vector of global selection indices, returns a vector of booleans
     * describing which indices of this dataset are selected. A locally selected
     * point will have true in its corresponding index, and false if not.
     * @param Vector of global selection indices
     * @param Boolean vector of locally selected points
     */
    void selectedLocalIndices(const std::vector<unsigned int>& selectionIndices, std::vector<bool>& selected) const;

    void getLocalSelectionIndices(std::vector<unsigned int>& localSelectionIndices) const;


public: // Action getters

    InfoAction& getInfoAction();
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
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
        * Save widget action to variant
        * @return Variant representation of the widget action
        */
    QVariantMap toVariantMap() const override;

public: // Linked data

    /**
     * Resolves linked data for the dataset
     * @param force Force update of all linked data (ignores linked data flags)
     */
    void resolveLinkedData(bool force = false) override;

public:

    std::vector<unsigned int> indices;

    InfoAction*                 _infoAction;                    /** Non-owning pointer to info action */
    mv::gui::GroupAction*       _dimensionsPickerGroupAction;   /** Group action for dimensions picker action */
    DimensionsPickerAction*     _dimensionsPickerAction;        /** Non-owning pointer to dimensions picker action */
    mv::EventListener           _eventListener;                 /** Listen to HDPS events */
};

// =============================================================================
// Factory
// =============================================================================

class PointDataFactory : public RawDataFactory
{
    Q_INTERFACES(mv::plugin::RawDataFactory mv::plugin::PluginFactory)
        Q_OBJECT
        Q_PLUGIN_METADATA(IID   "studio.manivault.PointData"
                          FILE  "PointData.json")

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
