#pragma once

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

class PointsPlugin : public RawData
{
public:
    PointsPlugin() : RawData("Points") { }
    ~PointsPlugin(void) override;
    
    void init() override;

    unsigned int getNumPoints() const
    {
        return data.size() / numDimensions;
    }

    unsigned int getNumDimensions() const
    {
        return numDimensions;
    }

    hdps::Set* createSet() const override;

    std::vector<QString> dimNames;
    std::vector<float> data;
    unsigned int numDimensions = 1;

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
