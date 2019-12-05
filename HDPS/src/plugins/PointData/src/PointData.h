#pragma once

#include "pointdata_export.h"

#include "RawData.h"

#include "Set.h"
#include <QString>
#include <QMap>
#include <QVariant>
#include <vector>

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

class POINTDATA_EXPORT PointData : public RawData
{
public:
    PointData() : RawData("Points") { }
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

    PointData& getRawData() const
    {
        return dynamic_cast<PointData&>(_core->requestRawData(getDataName()));
    }

    const std::vector<float>& getData() const
    {
        return getRawData().getData();
    }

    void setData(const float* data, unsigned int numPoints, unsigned int numDimensions)
    {
        getRawData().setData(data, numPoints, numDimensions);
    }

    unsigned int getNumPoints() const
    {
        if (isFull()) return getRawData().getNumPoints();
        else return indices.size();
    }

    unsigned int getNumDimensions() const
    {
        return getRawData().getNumDimensions();
    }

    const std::vector<QString>& getDimensionNames() const;

    void setDimensionNames(const std::vector<QString>& dimNames);

    // Constant subscript indexing
    const float& operator[](unsigned int index) const;

    // Subscript indexing
    float& operator[](unsigned int index);

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

    RawData* produce() override;
};
