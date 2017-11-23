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

DataTypePlugin* PointsPluginFactory::produce()
{
    return new PointsPlugin();
}
