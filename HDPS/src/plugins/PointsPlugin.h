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

	QVariant getMetaProperty(const QString& propertyName) const;
	void setMetaProperty(const QString& propertyName, const QVariant& propertyValue);
	bool hasProperty(const QString& propertyName) const;
	QStringList propertyNames() const;

private:
	QMap<QString, QVariant>		_metaData;
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
