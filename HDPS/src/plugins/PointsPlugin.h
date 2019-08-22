#pragma once

#include "pointsplugin_export.h"

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

class POINTSPLUGIN_EXPORT PointsPlugin : public RawData
{
public:
    PointsPlugin() : RawData("Points") { }
    ~PointsPlugin(void) override;
    
    void init() override;

    hdps::Set* createSet() const override;

    unsigned int getNumPoints() const
    {
        return _data.size() / _numDimensions;
    }

    unsigned int getNumDimensions() const
    {
        return _numDimensions;
    }

    const std::vector<float>& getData() const;

    const std::vector<QString>& getDimensionNames() const;

    void setData(float* data, unsigned int numPoints, unsigned int numDimensions);

    void setDimensionNames(const std::vector<QString>& dimNames);

    // Constant subscript indexing
    const float& operator[](unsigned int index) const;

    // Subscript indexing
    float& operator[](unsigned int index);
    

	QVariant getProperty(const QString & name) const
	{
		if (!hasProperty(name))
			return QVariant();

		return _properties[name];
	}

	void setProperty(const QString & name, const QVariant & value)
	{
		_properties[name] = value;
	}

	bool hasProperty(const QString & name) const
	{
		return _properties.contains(name);
	}

	QStringList propertyNames() const
	{
		return _properties.keys();
	}

private:
    /** Main store of point data in dimension-major order */
    std::vector<float> _data;

    /** Number of features of each data point */
    unsigned int _numDimensions = 1;

    std::vector<QString> _dimNames;
    
	QMap<QString, QVariant>		_properties;
};

class IndexSet : public hdps::Set
{
public:
    IndexSet(hdps::CoreInterface* core, QString dataName) : Set(core, dataName) { }
    ~IndexSet() override { }
    
    PointsPlugin& getData() const
    {
        return dynamic_cast<PointsPlugin&>(_core->requestData(getDataName()));
    }

    Set* copy() const override;

    std::vector<unsigned int> indices;
};

// =============================================================================
// Factory
// =============================================================================

class PointsPluginFactory : public RawDataFactory
{
    Q_INTERFACES(hdps::plugin::RawDataFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.PointsPlugin"
                      FILE  "PointsPlugin.json")
    
public:
    PointsPluginFactory(void) {}
    ~PointsPluginFactory(void) override {}
    
    RawData* produce() override;
};
