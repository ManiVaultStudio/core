#include "Points2DPlugin.h"

#include <QtCore>
#include <QtDebug>

Q_PLUGIN_METADATA(IID "nl.tudelft.Points2DPlugin")

// =============================================================================
// View
// =============================================================================

Points2DPlugin::~Points2DPlugin(void)
{
    
}

void Points2DPlugin::init()
{

}

// =============================================================================
// Factory
// =============================================================================

DataTypePlugin* Points2DPluginFactory::produce()
{
    return new Points2DPlugin();
}
