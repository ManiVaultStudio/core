#include "PointsPlugin.h"

#include <QtCore>
#include <QtDebug>

#include <cstring>

Q_PLUGIN_METADATA(IID "nl.tudelft.PointsPlugin")

// =============================================================================
// PointsPlugin
// =============================================================================

PointsPlugin::~PointsPlugin(void)
{
    
}

void PointsPlugin::init()
{

}

hdps::Set* PointsPlugin::createSet() const
{
    return new IndexSet(_core, getName());
}

unsigned int PointsPlugin::getNumPoints() const
{
    return _data.size() / _numDimensions;
}

unsigned int PointsPlugin::getNumDimensions() const
{
    return _numDimensions;
}

const std::vector<float>& PointsPlugin::getData() const
{
    return _data;
}

const std::vector<QString>& PointsPlugin::getDimensionNames() const
{
    return _dimNames;
}

void PointsPlugin::setData(const float* data, unsigned int numPoints, unsigned int numDimensions)
{
    _data.resize(numPoints * numDimensions);
    _numDimensions = numDimensions;

    if (data == nullptr)
        return;

    std::memcpy(_data.data(), data, sizeof(float) * numPoints * numDimensions);
}

void PointsPlugin::setDimensionNames(const std::vector<QString>& dimNames)
{
    _dimNames = dimNames;
}

// Constant subscript indexing
const float& PointsPlugin::operator[](unsigned int index) const
{
    return _data[index];
}

// Subscript indexing
float& PointsPlugin::operator[](unsigned int index)
{
    return _data[index];
}

// Temporary property metadata
QVariant PointsPlugin::getProperty(const QString & name) const
{
    if (!hasProperty(name))
        return QVariant();

    return _properties[name];
}

void PointsPlugin::setProperty(const QString & name, const QVariant & value)
{
    _properties[name] = value;
}

bool PointsPlugin::hasProperty(const QString & name) const
{
    return _properties.contains(name);
}

QStringList PointsPlugin::propertyNames() const
{
    return _properties.keys();
}

// =============================================================================
// IndexSet
// =============================================================================

hdps::Set* IndexSet::copy() const
{
    IndexSet* set = new IndexSet(_core, getDataName());
    set->setName(getName());
    set->indices = indices;
    return set;
}

// =============================================================================
// Factory
// =============================================================================

RawData* PointsPluginFactory::produce()
{
    return new PointsPlugin();
}
