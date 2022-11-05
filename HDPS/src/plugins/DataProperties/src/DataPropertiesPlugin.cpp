#include "DataProperties.h"

Q_PLUGIN_METADATA(IID "nl.BioVault.DataPropertiesPlugin")

using namespace hdps;

DataPropertiesPlugin::DataPropertiesPlugin(const PluginFactory* factory) :
    ViewPlugin(factory)
{
}

void DataPropertiesPlugin::init()
{
}

QIcon DataPropertiesPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("sliders", color);
}

ViewPlugin* DataPropertiesPluginFactory::produce()
{
    return new DataPropertiesPlugin(this);
}
