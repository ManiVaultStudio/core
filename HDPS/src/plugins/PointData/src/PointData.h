#pragma once

#include "pointdata_export.h"

#include "RawData.h"

#include "Set.h"

#include <biovault_bfloat16.h>

#include <QString>
#include <QMap>
#include <QVariant>

#include <array>
#include <cassert>
#include <utility> // For tuple.
#include <vector>

using namespace hdps::plugin;

namespace hdps
{
    // From "graphics/Vector2f.h"
    class Vector2f;
}

// =============================================================================
// Data Type
// =============================================================================

const hdps::DataType PointType = hdps::DataType(QString("Points"));

// =============================================================================
// Raw Data
// =============================================================================

#ifndef HDPS_ASSUME_FLOAT32_POINT_DATA
#define HDPS_ASSUME_FLOAT32_POINT_DATA [[deprecated("Deprecated: Assumes that point data is always 32-bit float!")]]
#endif

class POINTDATA_EXPORT PointData : public hdps::RawData
{
private:

    class VectorHolder
    {
    private:
        using TupleOfVectors = std::tuple <
            std::vector<float>,
            std::vector<biovault::bfloat16_t>,
            std::vector<std::int16_t>,
            std::vector<std::uint16_t>,
            std::vector<std::int8_t>,
            std::vector<std::uint8_t> >;
    public:
        enum class ElementTypeSpecifier
        {
            float32,
            bfloat16,
            int16,
            uint16,
            int8,
            uint8
        };

        static constexpr std::array<const char*, std::tuple_size<TupleOfVectors>::value> getElementTypeNames()
        {
            return
            {{
                "float32",
                "bfloat16",
                "int16",
                "uint16",
                "int8",
                "uint8"
            }};
        }

    private:
         // Tuple of vectors. Only the vector whose value_type corresponds to _elementTypeSpecifier
        // is selected. (The other vector is ignored, and could be cleared.) The vector stores the
        // point data in dimension-major order
        // Note: Instead of std::tuple, std::variant (from C++17) might be more appropriate, but at
        // the moment of writing, C++17 may not yet be enabled system wide.
        TupleOfVectors _tupleOfVectors;

        // Specifies which vector is selected, based on its value_type.
        ElementTypeSpecifier _elementTypeSpecifier{};

        // Tries to find the element type specifier that corresponds to ElementType.
        template <typename ElementType, typename Head, typename... Tail>
        constexpr static ElementTypeSpecifier recursiveFindElementTypeSpecifier(
            const std::tuple<Head, Tail...>*,
            const int temp = 0)
        {
            using HeadValueType = typename Head::value_type;

            if (std::is_same<ElementType, HeadValueType>::value)
            {
                return static_cast<ElementTypeSpecifier>(temp);
            }
            else
            {
                constexpr const std::tuple<Tail...>* tailNullptr{ nullptr };
                return recursiveFindElementTypeSpecifier<ElementType>(tailNullptr, temp + 1);
            }
        }


        template <typename ElementType>
        constexpr static ElementTypeSpecifier recursiveFindElementTypeSpecifier(const std::tuple<>*, const int)
        {
            return ElementTypeSpecifier{}; // Should not occur!
        }

        template <typename ReturnType, typename VectorHolderType, typename FunctionObject, typename Head, typename... Tail>
        static ReturnType recursiveVisit(VectorHolderType& vectorHolder, FunctionObject functionObject, const std::tuple<Head, Tail...>*)
        {
            using HeadValueType = typename Head::value_type;

            if (vectorHolder.template isSameElementType<HeadValueType>())
            {
                return functionObject(vectorHolder.template getVector<HeadValueType>());
            }
            else
            {
                constexpr const std::tuple<Tail...>* tailNullptr{nullptr};
                return recursiveVisit<ReturnType>(vectorHolder, functionObject, tailNullptr);
            }
        }

        template <typename ReturnType, typename VectorHolderType, typename FunctionObject>
        static ReturnType recursiveVisit(VectorHolderType&, FunctionObject&, const std::tuple<>*)
        {
            struct VisitException : std::exception
            {
                const char* what() const noexcept override
                {
                    return "visit error!";
                }
            };
            throw VisitException{};
        }

    public:

        /// Defaulted default-constructor. Ensures that the element type
        /// specifier is default-initialized (to "float32").
        VectorHolder() = default;

        /// Explicit constructor that copies the specified vector into the
        /// internal data structure. Ensures that the element type specifier
        /// matches the value type of the vector.
        template <typename T>
        explicit VectorHolder(const std::vector<T>& vec)
            :
            _elementTypeSpecifier{ getElementTypeSpecifier<T>() }
        {
            std::get<std::vector<T>>(_tupleOfVectors) = vec;
        }


        /// Explicit constructor that efficiently "moves" the specified vector
        /// into the internal data structure. Ensures that the element type
        /// specifier matches the value type of the vector.
        template <typename T>
        explicit VectorHolder(std::vector<T>&& vec)
            :
            _elementTypeSpecifier{ getElementTypeSpecifier<T>() }
        {
            std::get<std::vector<T>>(_tupleOfVectors) = std::move(vec);
        }


        // Similar to C++17 std::visit.
        template <typename ReturnType = void, typename FunctionObject>
        ReturnType constVisit(FunctionObject functionObject) const
        {
            constexpr const TupleOfVectors* const tupleNullptr{nullptr};
            return recursiveVisit<ReturnType>(*this, functionObject, tupleNullptr);
        }


        // Similar to C++17 std::visit.
        template <typename ReturnType = void, typename FunctionObject>
        ReturnType visit(FunctionObject functionObject)
        {
            constexpr const TupleOfVectors* const tupleNullptr{nullptr};
            return recursiveVisit<ReturnType>(*this, functionObject, tupleNullptr);
        }

        template <typename T>
        static constexpr ElementTypeSpecifier getElementTypeSpecifier()
        {
            constexpr const TupleOfVectors* const tupleNullptr{ nullptr };
            return recursiveFindElementTypeSpecifier<T>(tupleNullptr);
        }

        template <typename T>
        bool isSameElementType() const
        {
            constexpr auto elementTypeSpecifier = getElementTypeSpecifier<T>();
            return elementTypeSpecifier == _elementTypeSpecifier;
        }

        template <typename T>
        const std::vector<T>& getConstVector() const
        {
            // This function should only be used to access the currently selected vector.
            assert(isSameElementType<T>());
            return std::get<std::vector<T>>(_tupleOfVectors);
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

        /// Just forwarding to the corresponding member function of the currently selected std::vector.
        std::size_t size() const
        {
            return constVisit<std::size_t>([] (const auto& vec){ return vec.size(); });
        }

        /// Just forwarding to the corresponding member function of the currently selected std::vector.
        void resize(const std::size_t newSize)
        {
            visit([newSize](auto& vec) { vec.resize(newSize); });
        }
 
        /// Just forwarding to the corresponding member function of the currently selected std::vector.
        void clear()
        {
            visit([](auto& vec) { return vec.clear(); });
        }

        /// Just forwarding to the corresponding member function of the currently selected std::vector.
        void shrink_to_fit()
        {
            visit([](auto& vec) { return vec.shrink_to_fit(); });
        }

        void setElementTypeSpecifier(const ElementTypeSpecifier elementTypeSpecifier)
        {
            _elementTypeSpecifier = elementTypeSpecifier;
        }
 
        ElementTypeSpecifier getElementTypeSpecifier() const
        {
            return _elementTypeSpecifier;
        }


        /// Resizes the currently active internal data vector to the specified
        /// number of elements, and converts the elements of the specified data
        /// to the internal data element type, by static_cast. 
        template <typename T>
        void convertData(const T* const data, const std::size_t numberOfElements)
        {
            resize(numberOfElements);

            visit([data](auto& vec)
            {
                std::size_t i{};
                for (auto& elem: vec)
                {
                    elem = static_cast<std::remove_reference_t<decltype(elem)>>(data[i]);
                    ++i;
                }
            });
        }
    };


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
    using ElementTypeSpecifier = VectorHolder::ElementTypeSpecifier;

    PointData() : RawData("Points", PointType) { }
    ~PointData(void) override;

    void init() override;

    hdps::DataSet* createDataSet() const override;

    unsigned int getNumPoints() const;

    unsigned int getNumDimensions() const;

    // Similar to C++17 std::visit.
    template <typename ReturnType = void, typename FunctionObject>
    ReturnType constVisitFromBeginToEnd(FunctionObject functionObject) const
    {
        return _vectorHolder.constVisit<ReturnType>([functionObject](const auto& vec)
            {
                return functionObject(std::cbegin(vec), std::cend(vec));
            });
    }

    // Similar to C++17 std::visit.
    template <typename ReturnType = void, typename FunctionObject>
    ReturnType visitFromBeginToEnd(FunctionObject functionObject)
    {
        return _vectorHolder.visit<ReturnType>([functionObject](auto& vec)
            {
                return functionObject(std::begin(vec), std::end(vec));
            });
    }

    HDPS_ASSUME_FLOAT32_POINT_DATA const std::vector<float>& getData() const;

    void extractFullDataForDimension(std::vector<float>& result, const int dimensionIndex) const;
    void extractFullDataForDimensions(std::vector<hdps::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2) const;
    void extractDataForDimensions(std::vector<hdps::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2, const std::vector<unsigned int>& indices) const;

    template <typename ResultContainer, typename DimensionIndices>
    void populateFullDataForDimensions(ResultContainer& resultContainer, const DimensionIndices& dimensionIndices) const
    {
        CheckDimensionIndices(dimensionIndices);
        _vectorHolder.constVisit([&resultContainer, this, &dimensionIndices](const auto& vec)
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
            });
    }

    template <typename ResultContainer, typename DimensionIndices, typename Indices>
    void populateDataForDimensions(ResultContainer& resultContainer, const DimensionIndices& dimensionIndices, const Indices& indices) const
    {
        CheckDimensionIndices(dimensionIndices);

        _vectorHolder.constVisit([&resultContainer, this, &dimensionIndices, &indices](const auto& vec)
            {
                const std::ptrdiff_t numPoints{ getNumPoints() };
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
            });
    }

    const std::vector<QString>& getDimensionNames() const;

    static constexpr auto getElementTypeNames()
    {
        return VectorHolder::getElementTypeNames();
    }

    void setElementType(const ElementTypeSpecifier elementTypSpecifier)
    {
        if (_vectorHolder.getElementTypeSpecifier() != elementTypSpecifier)
        {
            _vectorHolder.clear();
            _vectorHolder.shrink_to_fit();
            _vectorHolder.setElementTypeSpecifier(elementTypSpecifier);
        }
    }

    template <typename T>
    void setElementType()
    {
        constexpr auto elementTypSpecifier = VectorHolder::getElementTypeSpecifier<T>();
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
        _vectorHolder.convertData(data, numPoints * numDimensions);
        _numDimensions = numDimensions;
    }

    /// Converts the specified data to the internal data, using static_cast for each data element.
    /// Convenience overload, allowing an std::vector or an std::array as
    /// input data container.
    template <typename T>
    void convertData(const T& inputDataContainer, const std::size_t numDimensions)
    {
        _vectorHolder.convertData(inputDataContainer.data(), inputDataContainer.size());
        _numDimensions = numDimensions;
    }

    /// Copies the specified data into the internal data, sets the number of
    /// dimensions as specified, and sets the selected internal data type
    /// according to the specified data type T.
    template <typename T>
    void setData(const T* const data, const std::size_t numPoints, const std::size_t numDimensions)
    {
         _vectorHolder = VectorHolder( std::vector<T>(data, data + numPoints * numDimensions) );
         _numDimensions = numDimensions;
    }


    /// Convenience overload to allow clearing the data by setData(nullptr, 0, numDimensions). 
    void setData(std::nullptr_t data, std::size_t numPoints, std::size_t numDimensions);


    /// Copies the data from the specified vector into the internal data, sets
    /// the number of dimensions as specified, and sets the selected internal
    /// data type according to the specified data type T.
    template <typename T>
    void setData(const std::vector<T>& data, const std::size_t numDimensions)
    {
        _vectorHolder = VectorHolder(data);
        _numDimensions = numDimensions;
    }

    /// Efficiently "moves" the data from the specified vector into the internal
    /// data, sets the number of dimensions as specified, and sets the selected
    /// internal data type according to the specified data type T.
    template <typename T>
    void setData(std::vector<T>&& data, const std::size_t numDimensions)
    {
        _vectorHolder = VectorHolder(std::move(data));
        _numDimensions = numDimensions;
    }

    void setDimensionNames(const std::vector<QString>& dimNames);

    // Constant subscript indexing
    HDPS_ASSUME_FLOAT32_POINT_DATA const float& operator[](unsigned int index) const;

    // Subscript indexing
    HDPS_ASSUME_FLOAT32_POINT_DATA float& operator[](unsigned int index);

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

public: // Properties

    /**
     * Get property in variant form
     * @param name Name of the property
     * @param defaultValue Default value
     * @return Property in variant form
     */
    QVariant getProperty(const QString& name, const QVariant& defaultValue = QVariant()) const
    {
        if (!hasProperty(name))
            return defaultValue;

        return _properties[name];
    }

    /**
    * Set property
    * @param name Name of the property
    * @param value Property value
    */
    void setProperty(const QString& name, const QVariant& value)
    {
        _properties[name] = value;
    }

    /**
    * Determines whether a property with a give name exists
    * @param name Name of the property
    * @param value If property with the given name exists
    */
    bool hasProperty(const QString& name) const
    {
        return _properties.contains(name);
    }

    /** Returns a list of available property names */
    QStringList propertyNames() const
    {
        return _properties.keys();
    }

private:
    VectorHolder _vectorHolder;

    /** Number of features of each data point */
    unsigned int _numDimensions = 1;

    std::vector<QString> _dimNames;

    QMap<QString, QVariant>    _properties;    /** Properties map */
};

// =============================================================================
// Point Set
// =============================================================================

class POINTDATA_EXPORT Points : public hdps::DataSet
{
public:
    Points(hdps::CoreInterface* core, QString dataName) : hdps::DataSet(core, dataName) { }
    ~Points() override { }

    /// Allows read-only access to the internal point data vector, from its begin to its end.
    /// Similar to C++17 std::visit.
    template <typename ReturnType = void, typename FunctionObject>
    ReturnType constVisitFromBeginToEnd(FunctionObject functionObject) const
    {
        return getRawData<PointData>().constVisitFromBeginToEnd<ReturnType>(functionObject);
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
        return getRawData<PointData>().visitFromBeginToEnd<ReturnType>(functionObject);
    }


    HDPS_ASSUME_FLOAT32_POINT_DATA const std::vector<float>& getData() const;

    /// Just calls the corresponding member function of its PointData.
    template <typename T>
    void convertData(const T* const data, const std::size_t numPoints, const std::size_t numDimensions)
    {
        getRawData<PointData>().convertData(data, numPoints, numDimensions);
    }


    /// Just calls the corresponding member function of its PointData.
    template <typename T>
    void convertData(const T& inputDataContainer, const std::size_t numDimensions)
    {
        getRawData<PointData>().convertData(inputDataContainer, numDimensions);
    }

    template <typename T>
    void setDataElementType()
    {
        getRawData<PointData>().setElementType<T>();
    }


    /// Just calls the corresponding member function of its PointData.
    template <typename T>
    void setData(const T* const data, const std::size_t numPoints, const std::size_t numDimensions)
    {
        getRawData<PointData>().setData(data, numPoints, numDimensions);
    }

    /// Just calls the corresponding member function of its PointData.
    void setData(std::nullptr_t data, std::size_t numPoints, std::size_t numDimensions);

    /// Just calls the corresponding member function of its PointData.
    template <typename T>
    void setData(const std::vector<T>& data, const std::size_t numDimensions)
    {
        getRawData<PointData>().setData(data, numDimensions);
    }

    /// Just calls the corresponding member function of its PointData.
    template <typename T>
    void setData(std::vector<T>&& data, const std::size_t numDimensions)
    {
        getRawData<PointData>().setData(std::move(data), numDimensions);
    }

    void extractDataForDimension(std::vector<float>& result, const int dimensionIndex) const;

    void extractDataForDimensions(std::vector<hdps::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2) const;

    /// Populates the specified result container with the data for the
    /// dimensions specified by the dimension indices.
    /// \note This function does not do any allocation. It assumes that the
    /// specified result container is large enough to store all the data.
    template <typename ResultContainer, typename DimensionIndices>
    void populateDataForDimensions(ResultContainer& resultContainer, const DimensionIndices& dimensionIndices) const
    {
        const auto& rawPointData = getRawData<PointData>();

        if (isFull())
        {
            rawPointData.populateFullDataForDimensions(resultContainer, dimensionIndices);
        }
        else
        {
            rawPointData.populateDataForDimensions(resultContainer, dimensionIndices, indices);
        }
    }


    unsigned int getNumPoints() const
    {
        if (isFull()) return getRawData<PointData>().getNumPoints();
        else return indices.size();
    }

    unsigned int getNumDimensions() const
    {
        return getRawData<PointData>().getNumDimensions();
    }

    const std::vector<QString>& getDimensionNames() const;

    void setDimensionNames(const std::vector<QString>& dimNames);

    // Constant subscript indexing
    HDPS_ASSUME_FLOAT32_POINT_DATA const float& operator[](unsigned int index) const;

    // Subscript indexing
    HDPS_ASSUME_FLOAT32_POINT_DATA float& operator[](unsigned int index);

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

public: // Properties

    /**
     * Get property in variant form
     * @param name Name of the property
     * @param defaultValue Default value
     * @return Property in variant form
     */
    QVariant getProperty(const QString& name, const QVariant& defaultValue = QVariant()) const
    {
        return getRawData<PointData>().getProperty(name, defaultValue);
    }

    /**
    * Set property
    * @param name Name of the property
    * @param value Property value
    */
    void setProperty(const QString& name, const QVariant& value)
    {
        getRawData<PointData>().setProperty(name, value);
    }

    /**
    * Determines whether a property with a give name exists
    * @param name Name of the property
    * @param value If property with the given name exists
    */
    bool hasProperty(const QString& name) const
    {
        return getRawData<PointData>().hasProperty(name);
    }

    /** Returns a list of available property names */
    QStringList propertyNames() const
    {
        return getRawData<PointData>().propertyNames();
    }

    // Set functions
    DataSet* copy() const override;

    void createSubset() const override;

    std::vector<unsigned int> indices;
};

// =============================================================================
// Factory
// =============================================================================

class PointDataFactory : public RawDataFactory
{
    Q_INTERFACES(hdps::plugin::RawDataFactory hdps::plugin::PluginFactory)
        Q_OBJECT
        Q_PLUGIN_METADATA(IID   "nl.tudelft.PointData"
            FILE  "PointData.json")

public:
    PointDataFactory(void) {}
    ~PointDataFactory(void) override {}

    hdps::RawData* produce() override;
};
