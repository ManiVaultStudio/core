#include "ExampleViewPlugin.h"

#include <QtCore>
#include <QtDebug>

Q_PLUGIN_METADATA(IID "nl.tudelft.ExampleViewPlugin")

// =============================================================================
// View
// =============================================================================

ExampleViewPlugin::~ExampleViewPlugin(void)
{
    
}

void ExampleViewPlugin::init()
{
}

void ExampleViewPlugin::dataAdded(const QString name)
{

}

void ExampleViewPlugin::dataChanged(const QString name)
{

}

void ExampleViewPlugin::dataRemoved(const QString name)
{

}

void ExampleViewPlugin::selectionChanged(const QString dataName)
{

}

QStringList ExampleViewPlugin::supportedDataKinds()
{
    QStringList supportedKinds;
    supportedKinds << "Points";
    return supportedKinds;
}

// =============================================================================
// Factory
// =============================================================================

ViewPlugin* ExampleViewPluginFactory::produce()
{
    return new ExampleViewPlugin();
}
