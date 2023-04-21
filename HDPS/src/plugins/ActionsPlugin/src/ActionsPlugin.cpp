#include "ActionsPlugin.h"

#include <Application.h>

Q_PLUGIN_METADATA(IID "nl.BioVault.ActionsPlugin")

using namespace hdps;

ActionsPlugin::ActionsPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _actionsWidget(&getWidget(), this)
{
}

void ActionsPlugin::init()
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(6, 6, 6, 6);

    layout->addWidget(&_actionsWidget);

    getWidget().setLayout(layout);
}

ActionsPluginFactory::ActionsPluginFactory() :
    ViewPluginFactory(true)
{
}

QIcon ActionsPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("cloud", color);
}

ViewPlugin* ActionsPluginFactory::produce()
{
    return new ActionsPlugin(this);
}
