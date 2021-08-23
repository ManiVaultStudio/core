#include "ClusterData.h"

#include <QtCore>
#include <QtDebug>

Q_PLUGIN_METADATA(IID "hdps.ClusterData")

// =============================================================================
// View
// =============================================================================

ClusterData::~ClusterData(void)
{
    
}

void ClusterData::init()
{

}

hdps::DataSet* ClusterData::createDataSet() const
{
    return new Clusters(_core, getName());
}

// =============================================================================
// Factory
// =============================================================================

hdps::plugin::RawData* ClusterDataFactory::produce()
{
    return new ClusterData(this);
}

QIcon Clusters::getIcon() const
{
    return QIcon();
}
