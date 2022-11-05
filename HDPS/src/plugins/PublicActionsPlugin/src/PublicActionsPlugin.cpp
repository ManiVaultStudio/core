#include "PublicActionsPlugin.h"

Q_PLUGIN_METADATA(IID "nl.BioVault.PublicActionsPlugin")

using namespace hdps;

PublicActionsPlugin::PublicActionsPlugin(const PluginFactory* factory) :
    ViewPlugin(factory)
{
    const_cast<PluginFactory*>(factory)->setMaximumNumberOfInstances(1);
}

void PublicActionsPlugin::init()
{
}

QIcon PublicActionsPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("play", color);
}

ViewPlugin* PublicActionsPluginFactory::produce()
{
    return new PublicActionsPlugin(this);
}
