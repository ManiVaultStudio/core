#include "PublicActionsPlugin.h"

Q_PLUGIN_METADATA(IID "nl.BioVault.PublicActionsPlugin")

using namespace hdps;

PublicActionsPlugin::PublicActionsPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _publicActionsWidget(nullptr)
{
    const_cast<PluginFactory*>(factory)->setMaximumNumberOfInstances(1);
}

void PublicActionsPlugin::init()
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addWidget(&_publicActionsWidget);

    getWidget().setLayout(layout);
}

QIcon PublicActionsPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("play", color);
}

ViewPlugin* PublicActionsPluginFactory::produce()
{
    return new PublicActionsPlugin(this);
}
