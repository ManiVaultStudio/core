#include "ClustersPlugin.h"

#include "PointsPlugin.h"

#include <QtCore>
#include <QtDebug>

Q_PLUGIN_METADATA(IID "nl.tudelft.ClustersPlugin")

// =============================================================================
// View
// =============================================================================

ClustersPlugin::~ClustersPlugin(void)
{
    
}

void ClustersPlugin::init()
{

}

hdps::Set* ClustersPlugin::createSet() const
{
    return new IndexSet();
}

// =============================================================================
// Factory
// =============================================================================

DataTypePlugin* ClustersPluginFactory::produce()
{
    return new ClustersPlugin();
}
