#include "PointData.h"

#include <QtCore>
#include <QtDebug>

#include <cstring>

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

RawData* PointDataFactory::produce()
{
    return new PointData();
}
