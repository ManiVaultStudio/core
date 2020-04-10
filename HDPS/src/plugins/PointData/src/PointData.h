#pragma once

#include "pointdata_export.h"

#include "RawData.h"

#include "Set.h"
#include <QString>
#include <QMap>
#include <QVariant>
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

class POINTDATA_EXPORT PointData : public hdps::RawData
{
public:
    PointData() : RawData("Points", PointType) { }
    ~PointData(void) override;

    void init() override;

    hdps::DataSet* createDataSet() const override;

    unsigned int getNumPoints() const;

    unsigned int getNumDimensions() const;

    const std::vector<float>& getData() const;

    const std::vector<QString>& getDimensionNames() const;

    void setData(const float* data, unsigned int numPoints, unsigned int numDimensions);

    void setDimensionNames(const std::vector<QString>& dimNames);

    // Constant subscript indexing
    const float& operator[](unsigned int index) const;

    // Subscript indexing
    float& operator[](unsigned int index);

    // Temporary property metadata
    QVariant getProperty(const QString & name) const;

    void setProperty(const QString & name, const QVariant & value);

    bool hasProperty(const QString & name) const;

    QStringList propertyNames() const;

private:
    /** Main store of point data in dimension-major order */
    std::vector<float> _data;

    /** Number of features of each data point */
    unsigned int _numDimensions = 1;

    std::vector<QString> _dimNames;

    QMap<QString, QVariant> _properties;
};

// =============================================================================
// Point Set
// =============================================================================

class POINTDATA_EXPORT Points : public hdps::DataSet
{
public:
    Points(hdps::CoreInterface* core, QString dataName) : hdps::DataSet(core, dataName) { }
    ~Points() override { }

    const std::vector<float>& getData() const
    {
        return getRawData<PointData>().getData();
    }

    void setData(const float* data, unsigned int numPoints, unsigned int numDimensions)
    {
        getRawData<PointData>().setData(data, numPoints, numDimensions);
    }

    void extractDimensions(std::vector<float>& result, const int dimensionIndex) const;

    void extractDimensions(std::vector<hdps::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2) const;

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
    const float& operator[](unsigned int index) const;

    // Subscript indexing
    float& operator[](unsigned int index);

    // Temporary property metadata
    QVariant getProperty(const QString& name) const
    {
        return getRawData<PointData>().getProperty(name);
    }

    void setProperty(const QString& name, const QVariant& value)
    {
        getRawData<PointData>().setProperty(name, value);
    }

    bool hasProperty(const QString& name) const
    {
        return getRawData<PointData>().hasProperty(name);
    }

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
