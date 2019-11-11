#include "ClusterData.h"

#include "IndexSet.h"

#include <QtCore>
#include <QtDebug>

Q_PLUGIN_METADATA(IID "nl.tudelft.ClusterData")

// =============================================================================
// View
// =============================================================================

ClusterData::~ClusterData(void)
{
    
}

void ClusterData::init()
{

}

hdps::Set* ClusterData::createSet() const
{
    return new hdps::IndexSet(_core, getName());
}

//void ClusterData::addCluster(IndexSet* cluster)
//{
//    clusters.push_back(cluster);
//}

// =============================================================================
// Factory
// =============================================================================

RawData* ClusterDataFactory::produce()
{
    return new ClusterData();
}
