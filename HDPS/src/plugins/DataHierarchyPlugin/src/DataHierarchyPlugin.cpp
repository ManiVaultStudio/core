#include "DataHierarchyPlugin.h"

Q_PLUGIN_METADATA(IID "nl.BioVault.DataHierarchyPlugin")

using namespace hdps;

DataHierarchyPlugin::DataHierarchyPlugin(const PluginFactory* factory) :
    ViewPlugin(factory)
{
    const_cast<PluginFactory*>(factory)->setMaximumNumberOfInstances(1);
}

void DataHierarchyPlugin::init()
{
}

QIcon DataHierarchyPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("sitemap", color);
}

ViewPlugin* DataHierarchyPluginFactory::produce()
{
    return new DataHierarchyPlugin(this);
}
