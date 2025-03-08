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

#include "event/EventListener.h"

#include <biovault_bfloat16/biovault_bfloat16.h>

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

    PointData(PluginFactory* factory) : RawData(factory, PointType) { }
    ~PointData(void) override;

    void init() override;

    mv::Dataset<mv::DatasetImpl> createDataSet(const QString& guid = "") const override;

    std::uint32_t getNumPoints() const;

    std::uint32_t getNumDimensions() const;

    std::uint64_t getNumberOfElements() const;

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
        return std::visit([functionObject](auto& vec) -> ReturnType
            {
                return functionObject(std::begin(vec), std::end(vec));
            },
            _variantOfVectors);
    }

    void extractFullDataForDimension(std::vector<float>& result, const int dimensionIndex) const;
    void extractFullDataForDimensions(std::vector<mv::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2) const;
    void extractDataForDimensions(std::vector<mv::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2, const std::vector<unsigned int>& indices) const;
    
    template <typename ResultContainer, typename DimensionIndices>
    void populateFullDataForDimensions(ResultContainer& resultContainer, const DimensionIndices& dimensionIndices) const
    {
        CheckDimensionIndices(dimensionIndices);
        std::visit([&resultContainer, this, &dimensionIndices](const auto& vec)
            {
                const std::ptrdiff_t numPoints{ getNumPoints() };
                std::ptrdiff_t resultIndex{};

                for (std::ptrdiff_t pointIndex{}; pointIndex < numPoints; ++pointIndex)
                {
                    const std::ptrdiff_t n{ pointIndex * _numDimensions };

                    for (const std::ptrdiff_t dimensionIndex : dimensionIndices)
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
        CheckDimensionIndices(dimensionIndices);

        std::visit([&resultContainer, this, &dimensionIndices, &indices](const auto& vec)
            {
                const std::ptrdiff_t numPoints{ static_cast<std::uint32_t>(indices.size()) };
                std::ptrdiff_t resultIndex{};

                for (std::ptrdiff_t pointIndex{}; pointIndex < numPoints; ++pointIndex)
                {
                    const std::ptrdiff_t n{ indices[pointIndex] * _numDimensions };

                    for (const std::ptrdiff_t dimensionIndex : dimensionIndices)
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
        return getElementTypeSpecifier();
    }

    void setElementType(const ElementTypeSpecifier elementTypSpecifier)
    {
        setElementTypeSpecifier(elementTypSpecifier);
    }

    template <typename T>
    void setElementType()
    {
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
        convertData(data, numPoints * numDimensions);
        _numDimensions = static_cast<std::uint32_t>(numDimensions);
    }

    /// Converts the specified data to the internal data, using static_cast for each data element.
    /// Convenience overload, allowing an std::vector or an std::array as
    /// input data container.
    template <typename T>
    void convertData(const T& inputDataContainer, const std::size_t numDimensions)
    {
        convertData(inputDataContainer.data(), inputDataContainer.size());
        _numDimensions = static_cast<std::uint32_t>(numDimensions);
    }

    /// Copies the specified data into the internal data, sets the number of
    /// dimensions as specified, and sets the selected internal data type
    /// according to the specified data type T.
    template <typename T>
    void setData(const T* const data, const std::size_t numPoints, const std::size_t numDimensions)
    {
         _variantOfVectors = VariantOfVectors( std::vector<T>(data, data + numPoints * numDimensions) );
         _numDimensions = static_cast<std::uint32_t>(numDimensions);
    }


    /// Convenience overload to allow clearing the data by setData(nullptr, 0, numDimensions). 
    void setData(std::nullptr_t data, std::size_t numPoints, std::size_t numDimensions);


    /// Copies the data from the specified vector into the internal data, sets
    /// the number of dimensions as specified, and sets the selected internal
    /// data type according to the specified data type T.
    template <typename T>
    void setData(const std::vector<T>& data, const std::size_t numDimensions)
    {
        _variantOfVectors = VariantOfVectors(data);
        _numDimensions = static_cast<unsigned int>(numDimensions);
    }

    /// Efficiently "moves" the data from the specified vector into the internal
    /// data, sets the number of dimensions as specified, and sets the selected
    /// internal data type according to the specified data type T.
    template <typename T>
    void setData(std::vector<T>&& data, const std::size_t numDimensions)
    {
        _variantOfVectors = VariantOfVectors(std::move(data));
        _numDimensions = static_cast<unsigned int>(numDimensions);
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
            points->_sparseData.setData(numRows, numCols, rowPointers, colIndices, values);
            points->_numRows = static_cast<uint32_t>(numRows);
            points->setData(std::vector<float> {}, numCols);
            points->_isDense = false;
        }

        static void setSparseData(PointData* points, size_t numRows, size_t numCols, std::vector<size_t>&& rowPointers, std::vector<size_t>&& colIndices, std::vector<float>&& values)
        {
            points->_sparseData.setData(numRows, numCols, std::move(rowPointers), std::move(colIndices), std::move(values));
            points->_numRows = static_cast<uint32_t>(numRows);
            points->setData(std::vector<float> {}, numCols);
            points->_isDense = false;
        }

        static SparseMatrix<size_t, size_t, float>& getSparseData(PointData* points)
        { 
            return points->_sparseData; 
        }

        static bool isDense(const PointData* points)
        {
            return points->_isDense;
        }

        static void setIsDense(PointData* points, bool isDense)
        {
            points->_isDense = isDense;
        }

        static size_t getNumNonZeroElements(const PointData* points)
        {
            return points->_sparseData.getNumNonZeros();
        }

        static std::vector<float> row(const PointData* points, size_t rowIndex)
        {
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
    VariantOfVectors _variantOfVectors;

    /** Number of features of each data point */
    unsigned int _numDimensions = 1;

    std::vector<QString> _dimNames;

private: // Sparse data, experimental
    unsigned int _numRows = 0;
    SparseMatrix<size_t, size_t, float> _sparseData = {};

    bool _isDense = true;
};

// =============================================================================
// Point Set
// =============================================================================

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

public:
    Points(QString dataName, bool mayUnderive = true, const QString& guid = "");
    ~Points() override;

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
        const auto notifyDimensionsChanged = numDimensions != getRawData<PointData>()->getNumDimensions();

        getRawData<PointData>()->setData(data, numPoints, numDimensions);

        if (notifyDimensionsChanged)
            mv::events().notifyDatasetDataDimensionsChanged(this);
    }

    /// Just calls the corresponding member function of its PointData.
    void setData(std::nullptr_t data, std::size_t numPoints, std::size_t numDimensions);

    /// Just calls the corresponding member function of its PointData.
    template <typename T>
    void setData(const std::vector<T>& data, const std::size_t numDimensions)
    {
        const auto notifyDimensionsChanged = numDimensions != getRawData<PointData>()->getNumDimensions();

        getRawData<PointData>()->setData(data, numDimensions);

        if (notifyDimensionsChanged)
            mv::events().notifyDatasetDataDimensionsChanged(this);
    }

    /// Just calls the corresponding member function of its PointData.
    template <typename T>
    void setData(std::vector<T>&& data, const std::size_t numDimensions)
    {
        const auto notifyDimensionsChanged = numDimensions != getRawData<PointData>()->getNumDimensions();

        getRawData<PointData>()->setData(std::move(data), numDimensions);

        if (notifyDimensionsChanged)
            mv::events().notifyDatasetDataDimensionsChanged(this);
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
