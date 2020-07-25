#ifndef HDPS_ASSUME_FLOAT32_POINT_DATA
// Avoid warnings about calls to deprecated PointData member functions.
#define HDPS_ASSUME_FLOAT32_POINT_DATA
#endif

#include "PointData.h"

#include <QtCore>
#include <QtDebug>

#include <cstring>
#include <type_traits>

#include "graphics/Vector2f.h"


Q_PLUGIN_METADATA(IID "nl.tudelft.PointData")

// =============================================================================
// PointData
// =============================================================================

PointData::~PointData(void)
{
    
}

void PointData::init()
{

}

hdps::DataSet* PointData::createDataSet() const
{
    return new Points(_core, getName());
}

unsigned int PointData::getNumPoints() const
{
    return _vectorHolder.size() / _numDimensions;
}

unsigned int PointData::getNumDimensions() const
{
    return _numDimensions;
}

const std::vector<QString>& PointData::getDimensionNames() const
{
    return _dimNames;
}

void PointData::setData(const std::nullptr_t, const std::size_t numPoints, const std::size_t numDimensions)
{
    _vectorHolder.resize(numPoints * numDimensions);
    _numDimensions = numDimensions;
}

void PointData::setDimensionNames(const std::vector<QString>& dimNames)
{
    _dimNames = dimNames;
}

float PointData::getValueAt(const std::size_t index) const
{
    return _vectorHolder.constVisit<float>([index](const auto& vec)
        {
            return vec[index];
        });
}

void PointData::setValueAt(const std::size_t index, const float newValue)
{
    _vectorHolder.visit([index, newValue](auto& vec)
        {
            using value_type = typename std::remove_reference_t<decltype(vec)>::value_type;
            vec[index] = static_cast<value_type>(newValue);
        });
}

void PointData::extractFullDataForDimension(std::vector<float>& result, const int dimensionIndex) const
{
    CheckDimensionIndex(dimensionIndex);

    result.resize(getNumPoints());

    _vectorHolder.constVisit(
        [&result, this, dimensionIndex](const auto& vec)
        {
            const auto resultSize = result.size();

            for (std::size_t i{}; i < resultSize; ++i)
            {
                result[i] = vec[i * _numDimensions + dimensionIndex];
            }
        });
}


void PointData::extractFullDataForDimensions(std::vector<hdps::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2) const
{
    CheckDimensionIndex(dimensionIndex1);
    CheckDimensionIndex(dimensionIndex2);

    result.resize(getNumPoints());

    _vectorHolder.constVisit(
        [&result, this, dimensionIndex1, dimensionIndex2](const auto& vec)
        {
            const auto resultSize = result.size();

            for (std::size_t i{}; i < resultSize; ++i)
            {
                const auto n = i * _numDimensions;
                result[i].set(vec[n + dimensionIndex1], vec[n + dimensionIndex2]);
            }
        });
}


void PointData::extractDataForDimensions(std::vector<hdps::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2, const std::vector<unsigned int>& indices) const
{
    CheckDimensionIndex(dimensionIndex1);
    CheckDimensionIndex(dimensionIndex2);

    result.resize(indices.size());

    _vectorHolder.constVisit(
        [&result, this, dimensionIndex1, dimensionIndex2, indices](const auto& vec)
        {
            const auto resultSize = result.size();

            for (std::size_t i{}; i < resultSize; ++i)
            {
                const auto n = std::size_t{ indices[i] } *_numDimensions;
                result[i].set(vec[n + dimensionIndex1], vec[n + dimensionIndex2]);
            }
        });
}

// =============================================================================
// Point Set
// =============================================================================


void Points::setData(std::nullptr_t, const std::size_t numPoints, const std::size_t numDimensions)
{
    getRawData<PointData>().setData(nullptr, numPoints, numDimensions);
}

void Points::extractDataForDimension(std::vector<float>& result, const int dimensionIndex) const
{
    // This overload assumes that the data set is "full".
    // Please remove the assert once non-full support is implemented (if necessary).
    assert(isFull());

    const auto& rawPointData = getRawData<PointData>();
    rawPointData.extractFullDataForDimension(result, dimensionIndex);
}


void Points::extractDataForDimensions(std::vector<hdps::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2) const
{
    const auto& rawPointData = getRawData<PointData>();

    if (isFull())
    {
        rawPointData.extractFullDataForDimensions(result, dimensionIndex1, dimensionIndex2);
    }
    else
    {
        rawPointData.extractDataForDimensions(result, dimensionIndex1, dimensionIndex2, indices);
    }
}


hdps::DataSet* Points::copy() const
{
    Points* set = new Points(_core, getDataName());
    set->setName(getName());
    set->indices = indices;
    return set;
}

void Points::createSubset() const
{
    const hdps::DataSet& selection = _core->requestSelection(getDataName());

    _core->createSubsetFromSelection(selection, *this, "Subset");
}

const std::vector<QString>& Points::getDimensionNames() const
{
    return getRawData<PointData>().getDimensionNames();
}

void Points::setDimensionNames(const std::vector<QString>& dimNames)
{
    getRawData<PointData>().setDimensionNames(dimNames);
}

float Points::getValueAt(const std::size_t index) const
{
    return getRawData<PointData>().getValueAt(index);
}

void Points::setValueAt(const std::size_t index, const float newValue)
{
    getRawData<PointData>().setValueAt(index, newValue);
}

// =============================================================================
// Factory
// =============================================================================

hdps::RawData* PointDataFactory::produce()
{
    return new PointData();
}
