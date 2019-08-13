#include "PointsPlugin.h"

#include <QtCore>
#include <QtDebug>

Q_PLUGIN_METADATA(IID "nl.tudelft.PointsPlugin")

// =============================================================================
// View
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

QVariant PointsPlugin::getMetaProperty(const QString & propertyName) const
{
	if (!hasProperty(propertyName))
		return QVariant();

	return _metaData[propertyName];
}

void PointsPlugin::setMetaProperty(const QString & propertyName, const QVariant & propertyValue)
{
	_metaData[propertyName] = propertyValue;
}

bool PointsPlugin::hasProperty(const QString & propertyName) const
{
	return _metaData.contains(propertyName);
}

QStringList PointsPlugin::propertyNames() const
{
	return _metaData.keys();
}

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
