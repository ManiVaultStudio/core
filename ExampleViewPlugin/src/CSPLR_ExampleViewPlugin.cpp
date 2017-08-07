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

void CSPLR_ExampleViewPlugin::dataAdded(const QString name)
{

}

void CSPLR_ExampleViewPlugin::dataChanged(const QString name)
{

}

void CSPLR_ExampleViewPlugin::dataRemoved(const QString name)
{

}

void CSPLR_ExampleViewPlugin::selectionChanged(const QString dataName)
{

}

QStringList CSPLR_ExampleViewPlugin::supportedDataKinds()
{
    QStringList supportedKinds;
    supportedKinds << "Points";
    return supportedKinds;
}

// =============================================================================
// Factory
// =============================================================================

ViewPlugin* CSPLR_ExampleViewPluginFactory::produce()
{
    return new CSPLR_ExampleViewPlugin();
}
