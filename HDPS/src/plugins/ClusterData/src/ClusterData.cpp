#include "ClusterData.h"
#include "InfoAction.h"

#include "Application.h"

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

void Clusters::init()
{
    _infoAction = QSharedPointer<InfoAction>::create(nullptr, _core, getName());

    addAction(*_infoAction.get());
}

QIcon Clusters::getIcon() const
{
    return hdps::Application::getIconFont("FontAwesome").getIcon("th-large");
}
