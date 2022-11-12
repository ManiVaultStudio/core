#include "LoggingPlugin.h"

Q_PLUGIN_METADATA(IID "nl.BioVault.LoggingWidget")

using namespace hdps;

LoggingPlugin::LoggingPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _loggingWidget(nullptr)
{
}

void LoggingPlugin::init()
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(6, 6, 6, 6);

    layout->addWidget(&_loggingWidget);

    getWidget().setLayout(layout);

    getWidget().setWindowTitle("Logging");
}

LoggingPluginFactory::LoggingPluginFactory() :
    ViewPluginFactory(true)
{
}

QIcon LoggingPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("scroll", color);
}

ViewPlugin* LoggingPluginFactory::produce()
{
    return new LoggingPlugin(this);
}
