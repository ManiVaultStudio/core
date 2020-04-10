#include "PointData.h"

#include <QtCore>
#include <QtDebug>

#include <cstring>

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
    return _data.size() / _numDimensions;
}

unsigned int PointData::getNumDimensions() const
{
    return _numDimensions;
}

const std::vector<float>& PointData::getData() const
{
    return _data;
}

const std::vector<QString>& PointData::getDimensionNames() const
{
    return _dimNames;
}

void PointData::setData(const float* data, unsigned int numPoints, unsigned int numDimensions)
{
    _data.resize(numPoints * numDimensions);
    _numDimensions = numDimensions;

    if (data == nullptr)
        return;

    std::memcpy(_data.data(), data, sizeof(float) * numPoints * numDimensions);
}

void PointData::setDimensionNames(const std::vector<QString>& dimNames)
{
    _dimNames = dimNames;
}

// Constant subscript indexing
const float& PointData::operator[](unsigned int index) const
{
    return _data[index];
}

// Subscript indexing
float& PointData::operator[](unsigned int index)
{
    return _data[index];
}

// Temporary property metadata
QVariant PointData::getProperty(const QString & name) const
{
    if (!hasProperty(name))
        return QVariant();

    return _properties[name];
}

void PointData::setProperty(const QString & name, const QVariant & value)
{
    _properties[name] = value;
}

bool PointData::hasProperty(const QString & name) const
{
    return _properties.contains(name);
}

QStringList PointData::propertyNames() const
{
    return _properties.keys();
}

// =============================================================================
// Point Set
// =============================================================================


void Points::extractDimensions(std::vector<float>& result, const int dimensionIndex) const
{
    // This overload assumes that the data set is "full".
    // Please remove the assert once non-full support is implemented (if necessary).
    assert(isFull());

    const auto& rawPointData = getRawData<PointData>();
    const std::ptrdiff_t numDimensions{ rawPointData.getNumDimensions() };

    assert(dimensionIndex >= 0);
    assert(dimensionIndex < numDimensions);

    const std::ptrdiff_t numPoints{ rawPointData.getNumPoints() };

    result.resize(numPoints);

    const auto& data = rawPointData.getData();

    for (std::ptrdiff_t i{}; i < numPoints; ++i)
    {
        result[i] = data[i * numDimensions + dimensionIndex];
    }
}


void Points::extractDimensions(std::vector<hdps::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2) const
{
    const auto& rawPointData = getRawData<PointData>();
    const std::ptrdiff_t numDimensions{ rawPointData.getNumDimensions() };

    assert(dimensionIndex1 >= 0);
    assert(dimensionIndex1 < numDimensions);
    assert(dimensionIndex2 >= 0);
    assert(dimensionIndex2 < numDimensions);

    // Note that Points::getNumPoints() returns the number of indices when the data set is not full.
    const std::ptrdiff_t numPoints{ getNumPoints() };
    result.resize(numPoints);

    const auto& data = rawPointData.getData();

    if (isFull())
    {
        for (std::ptrdiff_t i{}; i < numPoints; ++i)
        {
            const std::ptrdiff_t n{ i * numDimensions };
            result[i].set(data[n + dimensionIndex1], data[n + dimensionIndex2]);
        }
    }
    else
    {
        for (std::ptrdiff_t i{}; i < numPoints; ++i)
        {
            const std::ptrdiff_t n{ indices[i] * numDimensions };
            result[i].set(data[n + dimensionIndex1], data[n + dimensionIndex2]);
        }
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

    _core->createSubsetFromSelection(selection, getDataName(), "Subset");
}

const std::vector<QString>& Points::getDimensionNames() const
{
    return getRawData<PointData>().getDimensionNames();
}

void Points::setDimensionNames(const std::vector<QString>& dimNames)
{
    getRawData<PointData>().setDimensionNames(dimNames);
}

// Constant subscript indexing
const float& Points::operator[](unsigned int index) const
{
    return getRawData<PointData>()[index];
}

// Subscript indexing
float& Points::operator[](unsigned int index)
{
    return getRawData<PointData>()[index];
}

// =============================================================================
// Factory
// =============================================================================

hdps::RawData* PointDataFactory::produce()
{
    return new PointData();
}
