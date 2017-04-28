#include "CSPLR_ExampleViewPlugin.h"

#include <QtCore>
#include <QtDebug>

Q_PLUGIN_METADATA(IID "nl.tudelft.CSPLR_ExampleViewPlugin")

// =============================================================================
// View
// =============================================================================

CSPLR_ExampleViewPlugin::~CSPLR_ExampleViewPlugin(void)
{
    
}

void CSPLR_ExampleViewPlugin::init()
{
}

void CSPLR_ExampleViewPlugin::dataAdded(const DataTypePlugin& data)
{

}

void CSPLR_ExampleViewPlugin::dataChanged(const DataTypePlugin& data)
{

}

void CSPLR_ExampleViewPlugin::dataRemoved()
{

}

// =============================================================================
// Factory
// =============================================================================

ViewPlugin* CSPLR_ExampleViewPluginFactory::produce()
{
    return new CSPLR_ExampleViewPlugin();
}
