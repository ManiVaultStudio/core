#include "LoggingPlugin.h"

Q_PLUGIN_METADATA(IID "nl.BioVault.LoggingWidget")

using namespace hdps;

LoggingPlugin::LoggingPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _loggingWidget(nullptr)
{
    const_cast<PluginFactory*>(factory)->setMaximumNumberOfInstances(1);

    getAllowedDockingAreasAction().initialize({ "Left", "Right" }, { "Left", "Right" });
}

void LoggingPlugin::init()
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(6, 6, 6, 6);

    layout->addWidget(&_loggingWidget);

    getWidget().setLayout(layout);

    getWidget().setWindowTitle("Logging");
}

QIcon LoggingPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("scroll", color);
}

ViewPlugin* LoggingPluginFactory::produce()
{
    return new LoggingPlugin(this);
}
