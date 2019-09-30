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

const QStringList& PointsPlugin::getDimensionNames() const
{
    assert(_numDimensions == _dimNames.size());
    return _dimNames;
}

void PointsPlugin::setData(const float* data, unsigned int numPoints, unsigned int numDimensions, const QStringList& dimensionNames)
{
    _data.resize(numPoints * numDimensions);
    _numDimensions = numDimensions;
    auto numDimensionNames = dimensionNames.size();
    if (numDimensionNames != 0 && (numDimensionNames != _numDimensions))
    {
        qCritical() <<"The specified number of specified dimensions names differs from the specified number of dimensions";
    }
    if (numDimensionNames < _numDimensions)
    {
        _dimNames.reserve(_numDimensions);
        for (auto i = numDimensionNames; i < _numDimensions; ++i)
        {
            QString name = "Dim " + QString::number(i);
            _dimNames.push_back(name);
        }
    }
    else if (numDimensionNames > _numDimensions)
    {
        while (_dimNames.size() > _numDimensions)
            _dimNames.removeLast();
    }

    if (data == nullptr)
        return;

    std::memcpy(_data.data(), data, sizeof(float) * numPoints * numDimensions);
}

void PointsPlugin::setDimensionNames(const std::vector<QString>& dimNames)
{
    const auto numStoredDimNames = _dimNames.size();
    if (numStoredDimNames == 0 && _numDimensions==0)
    {
        _dimNames.reserve(dimNames.size());
        std::copy(dimNames.cbegin(), dimNames.cend(), std::back_inserter(_dimNames));
    }
    else if ((numStoredDimNames == _numDimensions) && (dimNames.size() == numStoredDimNames))
    {
        std::copy(dimNames.cbegin(), dimNames.cend(), _dimNames.begin());
    }
    else
    {
        qCritical() << "The number of dimension names differs from the number of dimensions";
    }
}
void PointsPlugin::setDimensionNames(const QStringList& dimNames)
{
    const auto numStoredDimNames = _dimNames.size();
    if (numStoredDimNames == 0 && _numDimensions == 0)
    {
        _dimNames == dimNames;
    }
    else if ((numStoredDimNames == _numDimensions) && (dimNames.size() == numStoredDimNames))
    {
    _dimNames = dimNames;
    }
    else
    {
        qCritical() << "The number of dimension names differs from the number of dimensions";
    }
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
