#include "PublicActionsPlugin.h"

Q_PLUGIN_METADATA(IID "nl.BioVault.PublicActionsPlugin")

using namespace hdps;

PublicActionsPlugin::PublicActionsPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _publicActionsWidget(nullptr)
{
    const_cast<PluginFactory*>(factory)->setMaximumNumberOfInstances(1);

    getAllowedDockingAreasAction().initialize({ "Left", "Right" }, { "Left", "Right" });
    getPreferredDockingAreaAction().setCurrentText("Right");
}

void PublicActionsPlugin::init()
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(6, 6, 6, 6);

    layout->addWidget(&_publicActionsWidget);

    getWidget().setLayout(layout);
}

QIcon PublicActionsPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("link", color);
}

ViewPlugin* PublicActionsPluginFactory::produce()
{
    return new PublicActionsPlugin(this);
}