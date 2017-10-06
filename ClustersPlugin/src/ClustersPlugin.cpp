#include "ClustersPlugin.h"

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
    return new ClusterSet();
}

void ClustersPlugin::addCluster(IndexSet* cluster)
{
    clusters.push_back(cluster);
}

hdps::Set* ClusterSet::copy() const
{
    ClusterSet* set = new ClusterSet();
    set->setName(getName());
    set->setDataName(getDataName());
    set->indices = indices;
    return set;
}


// =============================================================================
// Factory
// =============================================================================

DataTypePlugin* ClustersPluginFactory::produce()
{
    return new ClustersPlugin();
}
