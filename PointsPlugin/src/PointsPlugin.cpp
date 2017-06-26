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

hdps::Set* PointsPlugin::createSet()
{
    return new PointsSet();
}

// =============================================================================
// Factory
// =============================================================================

DataTypePlugin* PointsPluginFactory::produce()
{
    return new PointsPlugin();
}
